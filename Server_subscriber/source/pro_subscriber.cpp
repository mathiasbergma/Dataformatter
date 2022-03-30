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

//Please replace the following address with the address of your server
#define TOPIC 	"/gokart/#"
#define QOS		1

#define TIMEOUT "10"

using namespace std;

queue<string> q;
queue<string> power_msg_q;

MQTTClient client;
MQTTClient_deliveryToken token;
MQTTClient_message pubmsg = MQTTClient_message_initializer;

void convert_can_data(char *msg, struct can_data *data_frame, struct dbc_data*,
		int *count, struct converted_data *conv_data);
void load_dbc_data(struct dbc_data*);
int id_count_dbc_data();

void handler(int sig)
{
	char str[] = "Disconnecting mqtt client....goodbye\n";
	write(STDERR_FILENO, str, strlen(str));
	/********** Disconnect & destroy *********/
	MQTTClient_disconnect(client, 10000);
	MQTTClient_destroy(&client);

	exit(EXIT_SUCCESS);

}

int got_mail(void *context, char *topic, int topicLen, MQTTClient_message *msg)
{
	//time ( &rawtime );
	//timeinfo = localtime ( &rawtime );
	//strftime(todayDateStr, strlen("DD-MMM-YYYY HH:MM:SS")+1,"%d-%b-%Y %H:%M:%S",timeinfo);
	//fprintf(fp, "{\"timestamp\": \"%s\", \"topicid\": \"%s\", \"value\": \"%s\"} \n", todayDateStr, topic, (char *)msg->payload);

	//printf("Received on topic %s with len %d: %s\n", topic, msg->payloadlen,
	//(char*) msg->payload);

	// Prepare substrings for tokenization
	char *token = NULL;
	char *token2 = NULL;
	char *token3 = NULL;
	// Split the topic string and check to ensure that topic has the required format
	if ((token = strtok(topic, "/")) != NULL
			&& (token2 = strtok(NULL, "/")) != NULL
			&& (token3 = strtok(NULL, "/")) != NULL)
	{

		printf("%s\n", token);
		printf("%s\n", token2);
		printf("%s\n", token3);
		char combined_msg[200] = "";
		if (strcmp(token, "gokart") == 0)
		{
			strcat(combined_msg, token2);
			strcat(combined_msg, " ");
			strcat(combined_msg, (char*) msg->payload);

			printf("%s\n", combined_msg);
			if (strcmp(token3, "can") == 0)
			{
				q.push(combined_msg);
			}
			else if (strcmp(token3, "power") == 0)
			{
				power_msg_q.push(combined_msg);
			}
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
	struct converted_data conv_data;
	struct can_data data_frame;
	struct sigaction act;

	int id_count;
	struct dbc_data *dbc_array;

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
	conn_opts.ssl->enableServerCertAuth = 0;

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

	PGconn *conn = PQconnectdb(connInfo.c_str());
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

			// Convert value to network byte order
			to_nbo(conv_data.value, &bin_number);

			// Make postGres command
			const char command[] =
					"insert into canframes(gokart, signal, value, Unit) values($1, $2, $3::float8, $4);";
			char *signals = const_cast<char*>(conv_data.signal.c_str());
			char *unit = const_cast<char*>(conv_data.unit.c_str());
			int nParams = 4;
			const char *const paramValues[] =
			{ conv_data.gokart, signals, (char*) &bin_number, unit };
			const int paramLengths[] =
			{ sizeof(conv_data.gokart), sizeof(signals), sizeof(bin_number), sizeof(unit) };
			const int paramFormats[] =
			{ 0, 0, 1, 0 };
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

			q.pop();
			//cout << "Size of stack after pop: " << q.size() << endl;
		}
		while (!power_msg_q.empty())
		{
			// Make postGres command
			const char command[] =
					"insert into canframes(power_state) values($1);";
			char *message = const_cast<char*>(power_msg_q.front().c_str());
			int nParams = 1;
			const char *const paramValues[] =
			{ message };
			const int paramLengths[] =
			{ sizeof(message) };
			const int paramFormats[] =
			{ 0 };
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
			power_msg_q.pop();
		}
		sleep(1);
		//(void) pause();
	}

	return 0;
}
