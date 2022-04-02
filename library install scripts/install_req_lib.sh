#
#!/bin/bash

sudo apt update && sudo apt upgrade -y
sudo apt install vim mc
sudo apt install software-properties-common -y
sudo apt install git -y
sudo apt install python3-pip

sudo apt-get install usbutils
sudo apt-get install usb-modeswitch
sudo apt-get install ppp          

echo "reboot the system now please :)"
