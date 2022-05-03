/*
 * can_convert.h
 *
 *  Created on: Mar 23, 2022
 *      Author: bergma
 */

#ifndef CAN_CONVERT_H_
#define CAN_CONVERT_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <linux/can.h>
#include <time.h>
#include <unistd.h>		// sleep() function
#include <iostream>
#include <fstream>
#include <string>
#include <string.h>

#include <json-c/json.h>

using namespace std;

#define CHAR_PER_LINE	365
#define MAX_SIGNAL_IN_ID 4
#define SIGNAL_LENGTH 100
#define UNIT_LENGTH 100

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

void load_dbc_data(struct dbc_data *ptr);

void convert_can_data(char *msg, struct can_data *data_frame,
		struct dbc_data *dbc_array, int *count,
		struct converted_data_container *final_data);
void splitmsg(char *msg, struct can_data *data_frame, struct converted_data_container *final_data);

#endif /* CAN_CONVERT_H_ */
