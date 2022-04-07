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
char topic[50] = "/gokart/";

void read_configuration()
{
	string line;
	string delim = "	";
	ifstream file("/home/debian/Gokart_CAN_API/Power_off/server.conf");

	if (file.is_open())
	{
		while (!file.eof())
		{
			while (getline(file, line))
			{
				int start = 0;
				int end = line.find(delim);

				if (line.substr(start, end) == SERVER_H_LOOKUP)
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
					strcat(topic,
							line.substr(end + delim.size(), line.size() - 1).c_str());
					strcat(topic, "/power");
				}
				else if (line.substr(start, end) == TOPIC_LOOKUP)
				{
					strcpy(topic,
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

