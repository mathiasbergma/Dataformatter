/*
 * pro_subscriber.cpp
 *
 *  Created on: Mar 10, 2022
 *      Author: bergma
 */

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sstream>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <time.h>
#include <linux/can.h>
#include <queue>
#include <arpa/inet.h>
#include <postgresql/libpq-fe.h>
#include "MQTTClient.h"
#include <json-c/json.h>

#include "read_conf.h"
#include "can_convert.h"

static void to_nbo(double in, double *out)
{
	uint64_t *i = (uint64_t*) &in;
	uint32_t *r = (uint32_t*) out;

	/* convert input to network byte order */
	r[0] = htonl((uint32_t) ((*i) >> 32));
	r[1] = htonl((uint32_t) *i);
}

#define TOPIC 	"#"//"/gokart/#"
#define QOS		1

#define TIMEOUT "10"

using namespace std;

queue<string> q;
queue<string> power_msg_q;

MQTTClient client;
MQTTClient_deliveryToken token;
MQTTClient_message pubmsg = MQTTClient_message_initializer;

struct dbc_data *dbc_array;
PGconn *conn;

void handler(int sig)
{
	char str[] = "Disconnecting mqtt client....goodbye\n";
	write(STDERR_FILENO, str, strlen(str));
	/********** Disconnect & destroy *********/
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);
	free(dbc_array);
	PQfinish(conn);

	exit(EXIT_SUCCESS);

}

int got_mail(void *context, char *topic, int topicLen, MQTTClient_message *msg)
{
	// Prepare substrings for tokenization
	char *token = NULL;
	char *token2 = NULL;
	char *token3 = NULL;
	string buffer;
	printf("%s\n", topic);
	// Split the topic string and check to ensure that topic has the required format
	if ((token = strtok(topic, "/")) != NULL
			&& (token2 = strtok(NULL, "/")) != NULL
			&& (token3 = strtok(NULL, "/")) != NULL)
	{
		/* Create pointers to json that will hold received
		 * json and gokart id
		 */
		json_object *combined = json_object_new_object();

		if (strcmp(token, "gokart") == 0)
		{

			if (strcmp(token3, "can") == 0)
			{
				json_object *jobj;
				// Parse received transmission
				jobj = json_tokener_parse((char*) msg->payload);
				// Add key gokart and value id
				json_object_object_add(combined, "gokart",
						json_object_new_string(token2));
				// Add received json message to json object
				json_object_object_add(combined, "transmission", jobj);

				printf("%s\n", json_object_to_json_string_ext(combined,
				JSON_C_TO_STRING_PRETTY));

				// Prepare buffer for queue
				buffer = json_object_to_json_string_ext(combined,
				JSON_C_TO_STRING_PLAIN);

				// Push to queue
				q.push(buffer);
			}
			else if (strcmp(token3, "power") == 0)
			{
				// Create json object
				json_object *jobj = json_object_new_object();
				// Fill with message from transmission
				json_object_object_add(jobj, "power_state",
						json_object_new_string((char*) msg->payload));
				// Add key gokart & value id
				json_object_object_add(combined, "gokart",
						json_object_new_string(token2));
				// Add received message to json
				json_object_object_add(combined, "transmission", jobj);

				printf("%s\n", json_object_to_json_string_ext(combined,
				JSON_C_TO_STRING_PRETTY));

				power_msg_q.push(json_object_to_json_string_ext(combined,
				JSON_C_TO_STRING_PLAIN));
			}

			json_object_put(combined);
		}
	}

	MQTTClient_freeMessage(&msg);
	MQTTClient_free(topic);

	return 1;
}
void delivered(void *context, MQTTClient_deliveryToken tok)
{
	printf("Confirmed delivery of message with token value %d\n", tok);
	return;
}

int main(void)
{
	struct converted_data_container conv_data;
	struct can_data data_frame;
	struct sigaction act;

	int id_count;

	// Read configuration file
	read_configuration();

	/* count canid in csv file */
	id_count = id_count_dbc_data();

	// Allocate memory for the dbc_array
	dbc_array = (struct dbc_data*) malloc(id_count * sizeof(struct dbc_data));

	/* read dbc-data from csv file. Create array of structs */
	load_dbc_data(dbc_array);

	// Trap SIGINT : delivered on user pressing ^C
	memset(&act, 0, sizeof(act));
	act.sa_handler = handler;

	if (sigaction(SIGINT, &act, NULL) == -1)
		printf("sigaction SIGINT failed\n");

	/*********** Create MQTT Client ************/
	MQTTClient client;
	cout << "Attempting to create MQTT Client" << endl;
	MQTTClient_connectOptions conn_opts = MQTTClient_connectOptions_initializer;
	MQTTClient_SSLOptions ssl_opts = MQTTClient_SSLOptions_initializer;

	MQTTClient_create(&client, host, client_id, MQTTCLIENT_PERSISTENCE_NONE,
	NULL);
	conn_opts.keepAliveInterval = 20;
	conn_opts.cleansession = 1;
	conn_opts.ssl = &ssl_opts;
	conn_opts.ssl->enableServerCertAuth = 1;
	conn_opts.ssl->CApath = ca_path;
	conn_opts.ssl->keyStore = cert_path;
	conn_opts.ssl->privateKey = key_path;
	conn_opts.ssl->sslVersion = MQTT_SSL_VERSION_TLS_1_2;

	/*********** Necessary when using self-signed certificates *************/
	if (ssl_check == "NO")
	{
		conn_opts.ssl->enableServerCertAuth = 0;
	}

	MQTTClient_setCallbacks(client, NULL, NULL, got_mail, delivered);

	/*********** Connect MQTT Client ************/
	int rc;
	cout << "Connecting MQTT client" << endl;
	while ((rc = MQTTClient_connect(client, &conn_opts)) != MQTTCLIENT_SUCCESS)
	{
		cout << "Failed to connect, return code " << rc
				<< ", trying again in 2 sec" << endl;
		sleep(2);
	}

	printf("Subscribing to topic: %s\n", TOPIC);
	rc = MQTTClient_subscribe(client, TOPIC, QOS);

	printf("Subscribed to topic: %s, with return value: %d\n", TOPIC, rc);

	const std::string connInfo = "hostaddr=" + pg_host_addr + " port="
			+ pg_host_port + " dbname=" + pg_database_name + " user="
			+ pg_username + " password=" + pg_password + " connect_timeout="
			+ std::string(TIMEOUT);

	conn = PQconnectdb(connInfo.c_str());
	/* Check to see that the backend connection was successfully made */
	if (PQstatus(conn) != CONNECTION_OK)
	{
		std::cout << "Connection to database failed: " << PQerrorMessage(conn)
				<< std::endl;
		PQfinish(conn);
		return 1;
	}
	else
	{
		std::cout << "Connection to database succeed." << std::endl;
	}

	PGresult *res = NULL;
	double bin_number = 0;

	while (1)
	{
		while (!q.empty())
		{
			/***** Split one message and pop it from the queue afterwards *****/
			convert_can_data(const_cast<char*>(q.front().c_str()), &data_frame,
					dbc_array, &id_count, &conv_data);

			// Check if transmission was a position
			if (conv_data.can_id == 450)
			{
				// Split position values
				int lat_deg = conv_data.conv[0].value/100;
				double lat_min = conv_data.conv[0].value - lat_deg*100;
				int lon_deg = conv_data.conv[1].value/100;
				double lon_min = conv_data.conv[1].value - lon_deg*100;
				// Make a position string
				char pos[200];
				sprintf(pos,"%d %f,%d %f",lat_deg,lat_min,lon_deg,lon_min);

				// Make postGres command
				char command[200];
				sprintf(command,
						"insert into %s(gokart, signal, gps) values($1, $2, $3);",
						table_name);
				char *signals = const_cast<char*>(conv_data.conv[0].signal);
				int nParams = 3;
				const char *const paramValues[] =
				{ conv_data.gokart, signals, pos };
				const int paramLengths[] =
				{ sizeof(conv_data.gokart), sizeof(signals),
						sizeof(pos) };
				const int paramFormats[] =
				{ 0, 0, 0 };
				int resultFormat = 0;

				/* Execute postgres command */
				res = PQexecParams(conn, command, nParams, NULL,
						paramValues, paramLengths, paramFormats,
						resultFormat);
				if (PQresultStatus(res) != PGRES_COMMAND_OK)
				{
					std::cout << "PQexecParams failed: "
							<< PQresultErrorMessage(res) << std::endl;
				}
				PQclear(res);
			}
			//Transmission was not a positionF
			else
			{
				for (int i = 0; i < conv_data.size; i++)
				{

					// Convert value to network byte order
					to_nbo(conv_data.conv[i].value, &bin_number);

					// Make postGres command
					char command[200];
					sprintf(command,
							"insert into %s(gokart, signal, value, unit) values($1, $2, $3::float8, $4);",
							table_name);
					char *signals = const_cast<char*>(conv_data.conv[i].signal);
					char *unit = const_cast<char*>(conv_data.conv[i].unit);
					int nParams = 4;
					const char *const paramValues[] =
					{ conv_data.gokart, signals, (char*) &bin_number, unit };
					const int paramLengths[] =
					{ sizeof(conv_data.gokart), sizeof(signals),
							sizeof(bin_number), sizeof(unit) };
					const int paramFormats[] =
					{ 0, 0, 1, 0 };
					int resultFormat = 0;

					/* Execute postgres command */
					res = PQexecParams(conn, command, nParams, NULL,
							paramValues, paramLengths, paramFormats,
							resultFormat);
					if (PQresultStatus(res) != PGRES_COMMAND_OK)
					{
						std::cout << "PQexecParams failed: "
								<< PQresultErrorMessage(res) << std::endl;
					}
					PQclear(res);
				}
			}
			q.pop();
			//cout << "Size of stack after pop: " << q.size() << endl;
		}
		while (!power_msg_q.empty())
		{
			const char *token = NULL;
			const char *token2 = NULL;
			json_object *jobj;
			struct json_object *get_id;
			struct json_object *get_value;
			struct json_object *get_json;
			// Parse received transmission
			jobj = json_tokener_parse(
					const_cast<char*>(power_msg_q.front().c_str()));

			json_object_object_get_ex(jobj, "gokart", &get_id);
			json_object_object_get_ex(jobj, "transmission", &get_json);
			json_object_object_get_ex(get_json, "power_state", &get_value);

			token = json_object_get_string(get_id);
			token2 = json_object_get_string(get_value);

			if (token != NULL && token2 != NULL)
			{

				// Make postGres command
				char command[200];
				sprintf(command,
						"insert into %s(gokart, power_state) values($1, $2);",
						table_name);
				//char *message = token;
				int nParams = 2;
				const char *const paramValues[] =
				{ token, token2 };
				const int paramLengths[] =
				{ sizeof(token), sizeof(token2) };
				const int paramFormats[] =
				{ 0, 0 };
				int resultFormat = 0;

				/* Execute postgres command */
				res = PQexecParams(conn, command, nParams, NULL, paramValues,
						paramLengths, paramFormats, resultFormat);
				if (PQresultStatus(res) != PGRES_COMMAND_OK)
				{
					std::cout << "PQexecParams failed: "
							<< PQresultErrorMessage(res) << std::endl;
				}
				PQclear(res);
			}
			power_msg_q.pop();
		}
		usleep(200);
		//(void) pause();
	}

	return 0;
}
