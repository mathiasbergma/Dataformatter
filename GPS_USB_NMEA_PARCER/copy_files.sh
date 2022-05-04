#
#!/bin/bash

sudo cp 99-usb-serial.rules /etc/udev/rules.d/99-usb-serial.rules
echo "copied connection to /usr/bin/connection"
echo "finished"
echo "also ( sudo reboot )"

