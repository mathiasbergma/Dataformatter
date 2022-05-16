/*
 ============================================================================
 Name        : GPS_USB_NMEA_PARCER.c
 Author      : Michael Hynes
 Version     :
 Copyright   : free to use
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>

#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <linux/can.h>
#include <linux/can/raw.h>
#include <time.h>
#include <assert.h>
#include <sys/time.h>

#include "gps.h"

/*
	 * This section is for CAN bus socket setup
	*/
	int s;
	int numbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	const char *ifname = "can1";

void SendCANdata(void);

int main(int argc, char **argv)
{


	int fd;
	char buffer[255];
	int nbytes;
	int i;
	uint8_t t;
	char *field[20];

	frame.can_dlc = 8;

	if ((fd = OpenGPSPort("/dev/ttyACM0")) < 0)
	{
		perror("Cannot open GPS port");
		return 1;
	}

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("failed to open CAN Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));	// housekeeping
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		perror("failed Bind");
		return 1;
	}


	do {
		if ((nbytes = read(fd, &buffer, sizeof(buffer))) < 0) {
			perror("Read GPS fail");
			return 1;
		} else {
			if (nbytes == 0) {
				printf("No communication from GPS module\r\n");
				sleep(1);
			}
			else {
				buffer[nbytes - 1] = '\0';
				//printf("[%s]\r\n",buffer);
				if (checksum_valid(buffer)) {
					if ((strncmp(buffer, "$GP", 3) == 0) |
						(strncmp(buffer, "$GN", 3) == 0))
					{

						if (strncmp(&buffer[3], "GGA", 3) == 0) {
							i = parse_comma_delimited_str(buffer, field, 20);
							//debug_print_fields(i,field);
							//printf("UTC Time  :%s\r\n",field[1]);
							//printf("Latitude  :%s\r\n",field[2]);
							//printf("Longitude :%s\r\n",field[4]);
							//printf("Altitude  :%s\r\n",field[9]);
							//printf("Satellites:%s\r\n",field[7]);

							int latitude = strtod(field[2],NULL)*100000;  //100000 scale
							int longitude = strtod(field[4],NULL)*100000; //100000 scale
							//printf("%d",longitude);

							frame.can_id = 0x1C2;
							frame.can_dlc = 8;
							frame.data[0] = latitude >> 24;  //big endian
							frame.data[1] = latitude >> 16;
							frame.data[2] = latitude >> 8;
							frame.data[3] = latitude;
							frame.data[4] = longitude >> 24;
						    frame.data[5] = longitude >> 16;
						    frame.data[6] = longitude >> 8;
						    frame.data[7] = longitude;
						    SendCANdata();
						}
						if (strncmp(&buffer[3], "VTG", 3) == 0) {
							i = parse_comma_delimited_str(buffer, field, 20);
							//debug_print_fields(i,field);
							//printf("Speed     :%s\r\n",field[7]);  //km/t
							int speed = strtod(field[7],NULL)*1000;  //scaling 1000

							frame.can_id = 0x1C3;
							frame.can_dlc = 2;
							frame.data[0] = speed >> 8;
							frame.data[1] = speed;
							frame.data[2] = 0x00;
							frame.data[3] = 0x00;
							frame.data[4] = 0x00;
							frame.data[5] = 0x00;
							frame.data[6] = 0x00;
							frame.data[7] = 0x00;
							SendCANdata();

							//SetTime(field[9],field[1]);

							/* Send time over CANBUS.
							 * Approximately once every 30 seconds
							 */
							if (t>30){
								int timemsg = time(NULL);  //scaling 1

								frame.can_id = 0x1C4;
								frame.can_dlc = 4;
								frame.data[0] = timemsg >> 24;
								frame.data[1] = timemsg >> 16;
								frame.data[2] = timemsg >> 8;
								frame.data[3] = timemsg ;
								frame.data[4] = 0x00;
								frame.data[5] = 0x00;
								frame.data[6] = 0x00;
								frame.data[7] = 0x00;
								SendCANdata();

								t= 0;
							}
							else{
								t++;
							}

						}
					}
				}

			}
		}
	} while(1);

	if (close(fd) < 0) {
		perror("Close GPS error");
		return 1;
	}

	if (close(s)<0){
		perror("Close CAN socker error");
		return 1;
	}

	return (0);
}
void SendCANdata(void)
{
    numbytes = write(s, &frame, sizeof(frame));  //send message
    if (numbytes != sizeof(frame)) {
        perror("Send Error CAN frame");
    }
    printf("Sent id[0]=0x%X\n",frame.can_id);
}

