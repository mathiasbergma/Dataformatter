/*
 * read_conf.cpp
 *
 *  Created on: Mar 9, 2022
 *      Author: bergma
 */

#include "read_conf.h"

string host;
string ca_path;
string cert_path;
string key_path;
string client_id;

void read_configuration()
{
	ifstream conf_file;

	string line, str;
	string delimiter = "	";

	conf_file.open("/home/bergma/Gokart_CAN_API/Power_off/server.conf");

	while (std::getline(conf_file, line)) {
		int start = 0;
		int end = line.find(delimiter);
		// Split String by space in C++

		str = line.substr(start, end - start);

		start = end + delimiter.size();
		end = str.find(delimiter, start);

		if (str == SERVER_H_LOOKUP) {
			host = line.substr(start, end - start);
		}
		else if (str == CA_LOOKUP)
		{
			ca_path = line.substr(start, end - start);
		}
		else if (str == CERT_LOOKUP)
		{
			cert_path = line.substr(start, end - start);
		}
		else if (str == KEY_LOOKUP)
		{
			key_path = line.substr(start, end - start);
		}
		else if (str == CLIENT_ID_LOOKUP)
		{
			client_id = line.substr(start, end - start);
		}
	}
	conf_file.close();
}

