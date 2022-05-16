#
#!/bin/bash

sudo chmod +x /Debug/GPS_USB_NMEA_PARCER
sudo cp /Debug/GPS_USB_NMEA_PARCER /usr/bin/GPS_USB_NMEA_PARCER
echo "copied /Debug/GPS_USB_NMEA_PARCER to /usr/bin/GPS_USB_NMEA_PARCER"
sudo chmod +x GPS.service
sudo cp GPS.service /etc/systemd/system/GPS.service
echo "copied GPS.service to /etc/systemd/system/GPS.service"

echo "finished"
echo "enable service with systemctl for it to take effect :)"
echo "type ( sudo systemctl enable GPS.service ) "
echo "then ( sudo systemctl start GPS.service ) "

echo "also ( sudo reboot )"

