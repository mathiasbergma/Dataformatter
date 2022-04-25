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
	 * read dbc-data from text file. count lines in file
	 */

	FILE *pfile;
	pfile = fopen("gokart_server_config/DBC.csv", "r");

	if (pfile == NULL)
	{
		perror("file_open_read");
	}

	int count = 0;

	char line[CHAR_PER_LINE];

	/* read first line - not used */
	fgets(line, CHAR_PER_LINE, pfile);

	while (!feof(pfile))
	{
		fgets(line, CHAR_PER_LINE, pfile);
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

	char line[CHAR_PER_LINE];

	/* read first line - not used */
	fgets(line, CHAR_PER_LINE, pfile);

	while (!feof(pfile))
	{
		fgets(line, CHAR_PER_LINE, pfile);

		/* get signal count*/
		sscanf(line, "%05x, %d", &ptr->can_id, &ptr->signal_count);

// 0xID,SIGNAL COUNT,ENDIAN,SIGNAL(1),UNIT(1),START_BIT(1),STOP_BIT(1),OFFSET(1),SCALE(1)

		switch (ptr->signal_count)
		{
		case 1:
			sscanf(line, "%05X, %d, %d, "
					"%[^,], %[^,], %d, %d, %f, %f", &ptr->can_id,
					&ptr->signal_count, &ptr->big_endian,
					ptr->signal_info[0].signal_name, ptr->signal_info[0].unit,
					&ptr->signal_info[0].start_byte,
					&ptr->signal_info[0].stop_byte, &ptr->signal_info[0].offset,
					&ptr->signal_info[0].scale);
			printf("%X:\t %d\n",ptr->can_id,ptr->signal_count);
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
			sscanf(line, "%d, %d, %d,"
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

void convert_can_data(char *msg, struct can_data *data_frame,
		struct dbc_data *dbc_array, int *count,
		struct converted_data_container *final_data)
{

	splitmsg(msg, data_frame, final_data);

	/* arrays to hold temporary data */
	uint64_t raw_value_decimal[4] =
	{ 0 };
	double physical_value[4] =
	{ 0 };

	int i;
	int ii;
	int iii;
	int ns;
	int sec;
	time_t time;
	struct tm timeinfo;
	char buf[80];
	char timezone[10];

	for (i = 0; i < *count; i++)
	{

		if (data_frame->can_id == dbc_array[i].can_id)
		{

			for (ii = 0; ii < dbc_array[i].signal_count; ii++)
			{
				int num = 0;

				/* check endian notation and convert CAN-data from hex to decimal */
				if (dbc_array[i].big_endian == 1)
				{
					num = dbc_array[i].signal_info[ii].stop_byte
							- dbc_array[i].signal_info[ii].start_byte;  // 3
					for (iii = dbc_array[i].signal_info[ii].start_byte;
							iii < dbc_array[i].signal_info[ii].stop_byte + 1;
							iii++)
					{
						raw_value_decimal[ii] |= (data_frame->data[iii] & 0xff)
								<< num * 8;
						num--;

					}
				}
				else
				{
					for (iii = dbc_array[i].signal_info[ii].stop_byte + 1;
							iii > dbc_array[i].signal_info[ii].start_byte;
							iii--)
					{

						raw_value_decimal[ii] |= data_frame->data[iii]
								<< num * 8;
						num++;
					}
				}

				/* calculate physical value */
				physical_value[ii] = dbc_array[i].signal_info[ii].offset
						+ dbc_array[i].signal_info[ii].scale
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

			final_data->size = dbc_array[i].signal_count;
			for (int x = 0; x < dbc_array[i].signal_count; x++) {
				strcpy(final_data->conv[x].signal,
						dbc_array[i].signal_info[x].signal_name);
				strcpy(final_data->conv[x].unit, dbc_array[i].signal_info[x].unit);
				final_data->conv[x].value = physical_value[x];
				final_data->conv[x].timestamp = data_frame->timestamp;
			}

			for (int x = 0; x < dbc_array[i].signal_count; x++) {
				printf("%s: %.3f %s %Lf\n", final_data->conv[x].signal,
						final_data->conv[x].value, final_data->conv[x].unit,
						final_data->conv[x].timestamp);
			}

		}

		/* increment pointer in array */
		//dbc_array++;

	}
}
void splitmsg(char *msg, struct can_data *data_frame,
		struct converted_data_container *final_data)
{
	json_object *jobj;
	struct json_object *gokart_id;
	struct json_object *transmission;
	struct json_object *tmp;
	jobj = json_tokener_parse(msg);

	json_object_object_get_ex(jobj, "gokart", &gokart_id);
	json_object_object_get_ex(jobj, "transmission", &transmission);
	char can_msg_array[200];
	strcpy(can_msg_array, json_object_to_json_string_ext(transmission,
	JSON_C_TO_STRING_PLAIN));
	printf("%s\n", can_msg_array);

	strcpy(final_data->gokart, json_object_get_string(gokart_id));

	//tmp = json_object_array_get_idx(transmission, 1);

	sscanf(can_msg_array, "[%Lf,%d,\"%02X%02X%02X%02X%02X%02X%02X%02X\"]",
			&data_frame->timestamp, &data_frame->can_id, &data_frame->data[0],
			&data_frame->data[1], &data_frame->data[2], &data_frame->data[3],
			&data_frame->data[4], &data_frame->data[5], &data_frame->data[6],
			&data_frame->data[7]);
	printf("%X\n", data_frame->can_id);

}

