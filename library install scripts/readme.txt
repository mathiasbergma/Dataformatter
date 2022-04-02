an install script to install the required libraries for the project on the beagleboard.

sudo apt update
export LANG=en_DK.UTF-8
sudo apt-get install locales
sudo update-locale LANG=en_DK.UTF-8
sudo apt update && sudo apt upgrade -y
sudo apt install vim mc
sudo apt install software-properties-common -y
sudo apt install git -y
sudo apt install python3-pip

sudo apt-get install usbutils
sudo apt-get install usb-modeswitch
sudo apt-get install ppp          

echo "reboot the system now please :)"
