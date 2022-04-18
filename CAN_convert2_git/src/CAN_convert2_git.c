/*
 ============================================================================
 Name        : can_convert2.c
 Author      : Leo Pedersen
 Version     : 3.1
 Copyright   : Your copyright notice
 Description : convert can data to readable data
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/can.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>		// sleep() function

#define CHAR_PER_LINE	365
#define MAX_SIGNAL_IN_ID 4
#define SIGNAL_LENGTH 20
#define UNIT_LENGTH 20

struct signal {
	float offset;
	float scale;
	char unit[UNIT_LENGTH];
	char signal_name[SIGNAL_LENGTH];
	int start_byte;
	int stop_byte;
};

struct dbc_data {
	int signal_count;
	canid_t can_id;
	int big_endian;
	struct signal signal_info[MAX_SIGNAL_IN_ID];
};

struct can_data {
	long double timestamp;
	canid_t can_id;
	unsigned int data[8] __attribute__((aligned(8)));
};

struct converted_data {
	long double timestamp;
	char signal[SIGNAL_LENGTH];
	double value;
	char unit[UNIT_LENGTH];
};

struct converted_data_container {
	int size;
	char gokart[100];
	struct converted_data conv[MAX_SIGNAL_IN_ID];
};

int id_count_dbc_data();
void load_dbc_data(struct dbc_data*);
void convert_can_data(struct can_data *data_frame, struct dbc_data*, int *count);

int main(void) {

	/* struct to hold temporary can_data */
	struct can_data data_frame;

	/* count canid in csv file */
	int id_count = id_count_dbc_data();

	// Allocate memory for the dbc_array
	struct dbc_data *dbc_array;
	dbc_array = (struct dbc_data*) malloc(id_count * sizeof(struct dbc_data));

	/* read dbc-data from csv file. Create array of structs */
	load_dbc_data(dbc_array);

	while (1) {
		FILE *fp;
		if ((fp = fopen("can_data.csv", "r")) != NULL) {

			char line[CHAR_PER_LINE];

			/* read until end of can_data file */
			while (!feof(fp)) {

				/* read a line in file */
				fgets(line, CHAR_PER_LINE, fp);

				/* remove [ in line */
				char *newline = line + 1;

				/* data from file into struct */
				sscanf(newline, "%Lf,%03X,%02X%02X%02X%02X%02X%02X%02X%02X",
						&data_frame.timestamp, &data_frame.can_id,
						&data_frame.data[0], &data_frame.data[1],
						&data_frame.data[2], &data_frame.data[3],
						&data_frame.data[4], &data_frame.data[5],
						&data_frame.data[6], &data_frame.data[7]);

				/* covert can data function */
				convert_can_data(&data_frame, dbc_array, &id_count);
			}

			fclose(fp);

			/* delete file */
			remove("can_data.csv");

		} else {
			/* File not found, no memory leak since 'file' == NULL */
			printf("No file\n");
			sleep(1);
		}
	}
	return EXIT_SUCCESS;
}

int id_count_dbc_data() {
	/*
	 * read dbc-data from text file. count lines in file
	 */

	FILE *pfile;
	pfile = fopen("DBC.csv", "r");

	if (pfile == NULL) {
		perror("file_open_read");
	}

	int count = 0;

	char line[CHAR_PER_LINE];

	/* read first line - not used */
	fgets(line, CHAR_PER_LINE, pfile);

	while (!feof(pfile)) {
		fgets(line, CHAR_PER_LINE, pfile);
		count++;
	}

	fclose(pfile);
	return count;
}

void load_dbc_data(struct dbc_data *ptr) {
	/*
	 * read dbc-data from csv/text file. Create array of structs
	 */

	FILE *pfile;
	pfile = fopen("DBC.csv", "r");

	if (pfile == NULL) {
		perror("file_open_read");
	}

	char line[CHAR_PER_LINE];

	/* read first line - not used */
	fgets(line, CHAR_PER_LINE, pfile);

	while (!feof(pfile)) {
		fgets(line, CHAR_PER_LINE, pfile);

		/* get signal count*/
		sscanf(line, "%05x, %d", &ptr->can_id, &ptr->signal_count);

// 0xID,SIGNAL COUNT,ENDIAN,SIGNAL(1),UNIT(1),START_BIT(1),STOP_BIT(1),OFFSET(1),SCALE(1)

		switch (ptr->signal_count) {
		case 1:
			sscanf(line, "%05X, %d, %d, "
					"%[^,], %[^,], %d, %d, %f, %f", &ptr->can_id,
					&ptr->signal_count, &ptr->big_endian,
					ptr->signal_info[0].signal_name, ptr->signal_info[0].unit,
					&ptr->signal_info[0].start_byte,
					&ptr->signal_info[0].stop_byte, &ptr->signal_info[0].offset,
					&ptr->signal_info[0].scale);
			ptr++;
			break;

		case 2:
			sscanf(line, "%05X, %d, %d, "
					"%[^,], %[^,], %d, %d, %f, %f, "
					"%[^,], %[^,], %d, %d, %f, %f", &ptr->can_id,
					&ptr->signal_count, &ptr->big_endian,
					ptr->signal_info[0].signal_name, ptr->signal_info[0].unit,
					&ptr->signal_info[0].start_byte,
					&ptr->signal_info[0].stop_byte, &ptr->signal_info[0].offset,
					&ptr->signal_info[0].scale, ptr->signal_info[1].signal_name,
					ptr->signal_info[1].unit, &ptr->signal_info[1].start_byte,
					&ptr->signal_info[1].stop_byte, &ptr->signal_info[1].offset,
					&ptr->signal_info[1].scale);
			ptr++;
			break;

		case 3:
			sscanf(line, "%05X, %d, %d,"
					"%[^,], %[^,], %d, %d, %f, %f, "
					"%[^,], %[^,], %d, %d, %f, %f, "
					"%[^,], %[^,], %d, %d, %f, %f", &ptr->can_id,
					&ptr->signal_count, &ptr->big_endian,
					ptr->signal_info[0].signal_name, ptr->signal_info[0].unit,
					&ptr->signal_info[0].start_byte,
					&ptr->signal_info[0].stop_byte, &ptr->signal_info[0].offset,
					&ptr->signal_info[0].scale, ptr->signal_info[1].signal_name,
					ptr->signal_info[1].unit, &ptr->signal_info[1].start_byte,
					&ptr->signal_info[1].stop_byte, &ptr->signal_info[1].offset,
					&ptr->signal_info[1].scale, ptr->signal_info[2].signal_name,
					ptr->signal_info[2].unit, &ptr->signal_info[2].start_byte,
					&ptr->signal_info[2].stop_byte, &ptr->signal_info[2].offset,
					&ptr->signal_info[2].scale);
			ptr++;
			break;

		case 4:
			sscanf(line, "%05X, %d, %d,"
					"%[^,], %[^,], %d, %d, %f, %f, "
					"%[^,], %[^,], %d, %d, %f, %f, "
					"%[^,], %[^,], %d, %d, %f, %f, "
					"%[^,], %[^,], %d, %d, %f, %f", &ptr->can_id,
					&ptr->signal_count, &ptr->big_endian,
					ptr->signal_info[0].signal_name, ptr->signal_info[0].unit,
					&ptr->signal_info[0].start_byte,
					&ptr->signal_info[0].stop_byte, &ptr->signal_info[0].offset,
					&ptr->signal_info[0].scale, ptr->signal_info[1].signal_name,
					ptr->signal_info[1].unit, &ptr->signal_info[1].start_byte,
					&ptr->signal_info[1].stop_byte, &ptr->signal_info[1].offset,
					&ptr->signal_info[1].scale, ptr->signal_info[2].signal_name,
					ptr->signal_info[2].unit, &ptr->signal_info[2].start_byte,
					&ptr->signal_info[2].stop_byte, &ptr->signal_info[2].offset,
					&ptr->signal_info[2].scale, ptr->signal_info[3].signal_name,
					ptr->signal_info[3].unit, &ptr->signal_info[3].start_byte,
					&ptr->signal_info[3].stop_byte, &ptr->signal_info[3].offset,
					&ptr->signal_info[3].scale);
			ptr++;
			break;

		default:
			printf("No Signal data\n");
		}
	}

	fclose(pfile);
}

void convert_can_data(struct can_data *data_frame, struct dbc_data *dbc_array,
		int *count) {

	/* arrays to hold temporary data */
	uint64_t raw_value_decimal[4] = { 0 };
	double physical_value[4] = { 0 };

	/* struct to hold converted data */
	struct converted_data_container final_data;

	int i;
	int ii;
	int iii;
	int ns;
	int sec;
	time_t time;
	struct tm timeinfo;
	char buf[80];
	char timezone[10];

	for (i = 0; i < *count; i++) {

		if (data_frame->can_id == dbc_array->can_id) {

			for (ii = 0; ii < dbc_array->signal_count; ii++) {
				int num = 0;

				/* check endian notation and convert CAN-data from hex to decimal */
				if (dbc_array->big_endian == 1) {
					for (iii = dbc_array->signal_info[ii].start_byte;
							iii < dbc_array->signal_info[ii].stop_byte + 1;
							iii++) {
						raw_value_decimal[ii] |= data_frame->data[iii - 1]
								<< num * 8;
						num++;

					}
				} else {
					for (iii = dbc_array->signal_info[ii].stop_byte + 1;
							iii > dbc_array->signal_info[ii].start_byte;
							iii--) {

						raw_value_decimal[ii] |= data_frame->data[iii]
								<< num * 8;
						num++;
					}
				}

				/* calculate physical value */
				physical_value[ii] = dbc_array->signal_info[ii].offset
						+ dbc_array->signal_info[ii].scale
								* raw_value_decimal[ii];
			}

			/* timestamp to reable clock/time  */

			ns = ((long int) (1000000 * data_frame->timestamp)) % 1000000;
			sec = (int) data_frame->timestamp;

			time = sec;

			timeinfo = *localtime(&time);

			strftime(buf, sizeof(buf), "%A %d %B %Y %H:%M:%S", &timeinfo);
			strftime(timezone, sizeof(timezone), "%Z", &timeinfo);

			printf("%s.%03d %s\n", buf, ns, timezone);

			final_data.size = dbc_array->signal_count;

			for (i = 0; i < dbc_array->signal_count; i++) {
				strcpy(final_data.conv[i].signal,
						dbc_array->signal_info[i].signal_name);
				strcpy(final_data.conv[i].unit, dbc_array->signal_info[i].unit);
				final_data.conv[i].value = physical_value[i];
				final_data.conv[i].timestamp = data_frame->timestamp;
			}

			for (i = 0; i < dbc_array->signal_count; i++) {
				printf("%s: %.3f %s %Lf\n", final_data.conv[i].signal,
						final_data.conv[i].value, final_data.conv[i].unit,
						final_data.conv[i].timestamp);
			}

		}

		/* increment pointer in array */
		dbc_array++;
	}
}
