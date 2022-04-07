/*
 * can_convert.cpp
 *
 *  Created on: Mar 23, 2022
 *      Author: bergma
 */

#include "can_convert.h"



int id_count_dbc_data()
{
	/*
	 * read dbc-data from text file. Create array of structs
	 */

	FILE *pfile;
	pfile = fopen("gokart_server_config/DBC.csv", "r");

	if (pfile == NULL)
	{
		perror("file_open_read");
	}

	int count = 0;

	/* antal karaktere der kan være på en linje */
	char line[CHAR_PER_LINE];

	fgets(line, CHAR_PER_LINE, pfile); // læs føsrte linje i fil, skal ikke bruges til noget

	while (!feof(pfile))
	{
		fgets(line, CHAR_PER_LINE, pfile); // læs en linje i fil
		count++;
	}

	fclose(pfile);
	return count;
}
void load_dbc_data(struct dbc_data *ptr)
{
	/*
	 * read dbc-data from csv/text file. Create array of structs
	 */

	FILE *pfile;
	pfile = fopen("gokart_server_config/DBC.csv", "r");

	if (pfile == NULL)
	{
		perror("file_open_read");
	}

	/* antal karaktere der kan være på en linje */
	char line[CHAR_PER_LINE];

	fgets(line, CHAR_PER_LINE, pfile); // læs føsrte linje i fil, skal ikke bruges til noget

	while (!feof(pfile))
	{
		fgets(line, CHAR_PER_LINE, pfile); // læs en linje i fil

		sscanf(line, "%03X, %f, %d, %[^,], %[^,], %d, %d", &ptr->can_id,
				&ptr->offset, &ptr->scale, ptr->unit, ptr->signal,
				&ptr->start_bit, &ptr->stop_bit);
		ptr++;
	}

	fclose(pfile);

}

void convert_can_data(char *msg, struct can_data *data_frame, struct dbc_data* dbc_array,
		int *count, struct converted_data *conv_data)
{

	splitmsg(msg, data_frame);

	//int count1 = *count;

	int i;
	for (i = 0; i < *count; i++)
	{

		if (data_frame->can_id == dbc_array[i].can_id)
		{
			/* casting hex to decimal */
			uint64_t raw_value_decimal = 0;

			int num = 0;
			int ii;
			for (ii = dbc_array[i].start_bit; ii < dbc_array[i].stop_bit + 1; ii++)
			{
				raw_value_decimal |= data_frame->data[ii] << num * 8;
				num++;
			}

			/* timestamp to reable clock/time  */
			int ms = data_frame->timestamp % 1000;
			int sec = data_frame->timestamp / 1000;

			time_t time = sec;

			struct tm timeinfo;
			timeinfo = *localtime(&time);
			char buf[80];

			strftime(buf, sizeof(buf), "%A %d %B %Y %H:%M:%S", &timeinfo);

			float physical_value = dbc_array[i].offset
					+ (float)raw_value_decimal / (float)dbc_array[i].scale;

			/* print data */
			printf("%s.%d	%s: %.3f %s\n", buf, ms, dbc_array[i].signal,
					physical_value, dbc_array->unit);

			printf("\n");

			conv_data->timestamp = data_frame->timestamp;
			conv_data->signal = dbc_array[i].signal;
			conv_data->value = physical_value;
			conv_data->unit = dbc_array[i].unit;
			strcpy(conv_data->gokart, data_frame->gokart);

		}

		/* increment pointer in array */
		//dbc_array++;
	}
}
void splitmsg(char *msg, struct can_data *data_frame)
{
	char *token = NULL;
	char *token2 = NULL;
	// Split the topic string and check to ensure that topic has the required format
	token = strtok(msg, " ");
	strcpy(data_frame->gokart, token);
	token2 = strtok(NULL, " ");
	sscanf(token2,
			"{\"timestamp\":%llu,\"canid\":%05X,\"dataframe\":[%03X,%03X,%03X,%03X,%03X,%03X,%03X,%03X]}",
			&data_frame->timestamp, &data_frame->can_id, &data_frame->data[0],
			&data_frame->data[1], &data_frame->data[2], &data_frame->data[3],
			&data_frame->data[4], &data_frame->data[5], &data_frame->data[6],
			&data_frame->data[7]);
	printf("%X\n",data_frame->can_id);
}




