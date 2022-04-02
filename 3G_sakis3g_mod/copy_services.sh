#
#!/bin/bash

sudo cp connection /usr/bin/connection
echo "copied connection to /usr/bin/connection"
sudo cp NET_CONNECT /usr/bin/NET_CONNECT
echo "copied NET_CONNECT to /etc/bin/NET_CONNECT"
sudo cp connection.service /etc/systemd/system/connection.service
echo "copied connection.service to /etc/systemd/system/connection.service"

echo "finished"
echo "enable service with systemctl for it to take effect :)"

