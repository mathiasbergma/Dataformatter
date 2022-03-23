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

#define CHAR_PER_LINE	200

using namespace std;

struct dbc_data
{
	canid_t can_id;
	float offset;
	int scale;
	char unit[20];
	char signal[20];
	int start_bit;
	int stop_bit;
};

struct can_data
{
	unsigned long long timestamp;
	canid_t can_id;
	unsigned int data[8] __attribute__((aligned(8)));
};

struct converted_data
{
	unsigned long long timestamp;
	string signal;
	double value;
	string unit;
};

int id_count_dbc_data();

void load_dbc_data(struct dbc_data *ptr);

void convert_can_data(char *msg, struct can_data *data_frame, struct dbc_data* dbc_array,
		int *count, struct converted_data *conv_data);

void splitmsg(char *msg, struct can_data *data_frame);

#endif /* CAN_CONVERT_H_ */
