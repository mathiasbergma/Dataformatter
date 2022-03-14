/*
 ============================================================================
 Name        : CAN_read.c
 Author      : Leo
 Version     : 1.0
 Copyright   : open
 Description : can bus read (candump)
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

const char *JSON_FORMAT_OUT =
		"{\"timestamp\": %lld, \"ID\": 0x%03X, \"data\": %02X %02X %02X %02X %02X %02X %02X %02X}";

const char *CSV_FORMAT_OUT = "%lld,0x%03X,%02X %02X %02X %02X %02X %02X %02X %02X";

void print_can_data(const struct can_frame *frame);
void write_to_file(struct can_frame *frame);

long long current_timestamp() {
	struct timeval te;
	gettimeofday(&te, NULL); 	// get current itme
	long long milliseconds = (unsigned long long) (te.tv_sec) * 1000
			+ (unsigned long long) (te.tv_usec) / 1000; // calculate milliseconds
	return milliseconds;
}

int main(void) {

	/*
	 * This section is for CAN bus socket setup
	 */
	int s;
	int nbytes;
	struct sockaddr_can addr;
	struct ifreq ifr;
	struct can_frame frame;
	const char *ifname = "can0";

	printf("CAN Sockets Receive Demo\r\n");

	if ((s = socket(PF_CAN, SOCK_RAW, CAN_RAW)) < 0) {
		perror("Socket");
		return 1;
	}

	strcpy(ifr.ifr_name, ifname);
	ioctl(s, SIOCGIFINDEX, &ifr);

	memset(&addr, 0, sizeof(addr));	// housekeeping
	addr.can_family = AF_CAN;
	addr.can_ifindex = ifr.ifr_ifindex;

	if (bind(s, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
		perror("Bind");
		return 1;
	}

	/*
	 * This section is the can-dump
	 */
	while (1) {

		/* read CAN-bus*/
		nbytes = read(s, &frame, sizeof(struct can_frame));

		if (nbytes < 0) {
			perror("Read");
			return 1;
		}

		/* print can-bus data to console */
		print_can_data(&frame);

		/* write to file function */
		write_to_file(&frame);

	}

	if (close(s) < 0) {
		perror("Close");
		return 1;
	}

	return 0;
}

void print_can_data(const struct can_frame *frame) {

	/* Timestamp */
	long long timestamp = current_timestamp();
	printf("Timestamp: %lld     ", timestamp);

	/* printing CAN_identifier and CAN_Control_field */
	//printf("0x%03X [%d] ", frame.can_id, frame.can_dlc);
	printf("0x%03X [%d] ", frame->can_id, frame->can_dlc);

	/* printing CAN_Data_Field */
	for (int i = 0; i < frame->can_dlc; i++) {
		printf("%02X ", frame->data[i]);
	}

	printf("\r\n");
}

void write_to_file(struct can_frame *frame) {
	//
	// This section is the file dump
	//

	FILE *fptr;
	fptr = fopen("can_data.csv", "a");
	// fptr = fopen("data.txt", "w");

	if (fptr == NULL) {
		perror("file_open_write");
	}

	fprintf(fptr, CSV_FORMAT_OUT, current_timestamp(), frame->can_id,
			frame->data[0], frame->data[1], frame->data[2], frame->data[3],
			frame->data[4], frame->data[5], frame->data[6], frame->data[7]);
	fprintf(fptr, "\r\n");

	fclose(fptr);

}
