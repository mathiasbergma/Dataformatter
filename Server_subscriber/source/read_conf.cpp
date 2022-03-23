/*
 * read_conf.cpp
 *
 *  Created on: Mar 9, 2022
 *      Author: bergma
 */

#include "read_conf.h"

char host[100];
char ca_path[60];
char cert_path[60];
char key_path[60];
char client_id[60];
string pg_username;
string pg_host_addr;
string pg_host_port;
string pg_database_name;
string pg_password;

void read_configuration()
{
	FILE *conf_file = fopen("gokart_server_config/server.conf", "r");
	string line;
	string delim = "	";
	ifstream file("gokart_server_config/server.conf");

	if (file.is_open())
	{
		while (!file.eof())
		{
			while (getline(file, line))
			{
				int start = 0;
				int end = line.find(delim);

				if (line.substr(start, end) == USERNAME_LOOKUP)
				{
					pg_username = line.substr(end + delim.size(),
							line.size() - 1);
				}
				else if (line.substr(start, end) == PASSWORD_LOOKUP)
				{
					pg_password = line.substr(end + delim.size(),
							line.size() - 1);
				}
				else if (line.substr(start, end) == HOST_ADDR_LOOKUP)
				{
					pg_host_addr = line.substr(end + delim.size(),
							line.size() - 1);
				}
				else if (line.substr(start, end) == HOST_PORT_LOOKUP)
				{
					pg_host_port = line.substr(end + delim.size(),
							line.size() - 1);
				}
				else if (line.substr(start, end) == DATABASE_NAME_LOOKUP)
				{
					pg_database_name = line.substr(end + delim.size(),
							line.size() - 1);
				}
				else if (line.substr(start, end) == SERVER_H_LOOKUP)
				{
					strcpy(host,
							line.substr(end + delim.size(), line.size() - 1).c_str());

				}
				else if (line.substr(start, end) == CA_LOOKUP)
				{
					strcpy(ca_path,
							line.substr(end + delim.size(), line.size() - 1).c_str());

				}
				else if (line.substr(start, end) == CERT_LOOKUP)
				{
					strcpy(cert_path,
							line.substr(end + delim.size(), line.size() - 1).c_str());
				}
				else if (line.substr(start, end) == KEY_LOOKUP)
				{
					strcpy(key_path,
							line.substr(end + delim.size(), line.size() - 1).c_str());
				}
				else if (line.substr(start, end) == CLIENT_ID_LOOKUP)
				{
					strcpy(client_id,
							line.substr(end + delim.size(), line.size() - 1).c_str());
				}
			}
		}
		file.close();
	}

	remove_trailing(host);
	remove_trailing(ca_path);
	remove_trailing(cert_path);
	remove_trailing(key_path);
	remove_trailing(client_id);

}
void remove_trailing(char *buffer)
{
	int len = strlen(buffer);
	if (len > 0 && buffer[len - 1] == '\n')
		buffer[len - 1] = 0;
}

