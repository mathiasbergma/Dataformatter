/*
 * read_conf.h
 *
 *  Created on: Mar 9, 2022
 *      Author: bergma
 */

#ifndef READ_CONF_H_
#define READ_CONF_H_
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

#define SERVER_H_LOOKUP		"server_hostname"
#define CA_LOOKUP			"server_ca_path"
#define CERT_LOOKUP			"client_cert_path"
#define KEY_LOOKUP			"client_key_path"
#define CLIENT_ID_LOOKUP	"friendly_name"
#define USERNAME_LOOKUP 	"postgres_user"
#define PASSWORD_LOOKUP 	"postgres_password"
#define HOST_ADDR_LOOKUP 	"postgres_hostname"
#define HOST_PORT_LOOKUP 	"postgres_port"
#define DATABASE_NAME_LOOKUP "postgres_db_name"
#define TOPIC_LOOKUP		"topic"
#define DATABASE_TABLE_NAME	"postgres_table_name"

extern char host[100];
extern char ca_path[60];
extern char cert_path[60];
extern char key_path[60];
extern char client_id[60];
extern char topic[50];
extern char table_name[50];

extern string pg_username;
extern string pg_host_addr;
extern string pg_host_port;
extern string pg_database_name;
extern string pg_password;

void read_configuration();
void remove_trailing(char * buffer);



#endif /* READ_CONF_H_ */
