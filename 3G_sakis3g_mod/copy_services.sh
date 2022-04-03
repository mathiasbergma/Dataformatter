#
#!/bin/bash

sudo chmod +x is_there_internet
sudo cp is_there_internet /usr/bin/is_there_internet
sudo chmod +x connection
sudo cp connection /usr/bin/connection
echo "copied connection to /usr/bin/connection"
sudo chmod +x fixusb
sudo cp fixusb /usr/bin/fixusb
echo "copied fixusb to /usr/bin/fixusb"
sudo chmod +x NET_CONNECT
sudo cp NET_CONNECT /usr/bin/NET_CONNECT
echo "copied NET_CONNECT to /usr/bin/NET_CONNECT"
sudo chmod +x sakis3g
sudo cp sakis3g /usr/bin/sakis3g
echo "copied sakis3g to /usr/bin/sakis3g"
sudo chmod +x connection.service
sudo cp connection.service /etc/systemd/system/connection.service
echo "copied connection.service to /etc/systemd/system/connection.service"

echo "finished"
echo "enable service with systemctl for it to take effect :)"
echo "type ( sudo systemctl enable connection.service ) "
echo "then ( sudo systemctl start connection.service ) "

echo "also ( sudo reboot )"

