/*
 * read_conf.h
 *
 *  Created on: Mar 9, 2022
 *      Author: bergma
 */

#ifndef READ_CONF_H_
#define READ_CONF_H_

#include <fstream>
#include <string>

using namespace std;

#define SERVER_H_LOOKUP		"server_hostname"
#define CA_LOOKUP			"server_ca_path"
#define CERT_LOOKUP			"client_cert_path"
#define KEY_LOOKUP			"client_key_path"
#define CLIENT_ID_LOOKUP	"friendly_name"

extern string host;
extern string ca_path;
extern string cert_path;
extern string key_path;
extern string client_id;

void read_configuration();



#endif /* READ_CONF_H_ */
