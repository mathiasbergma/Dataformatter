#
#!/bin/bash

sudo chmod +x ./CANsniff.py
sudo cp ./CANsniff.py /usr/bin/CANsniff.py
echo "copied CANsniff.py to /usr/bin/CANsniff.py"
sudo chmod +x CAN_MQTT.service
sudo cp CAN_MQTT.service /etc/systemd/system/CAN_MQTT.service
echo "copied CAN_MQTT.service to /etc/systemd/system/CAN_MQTT.service"

echo "finished"
echo "enable service with systemctl for it to take effect :)"
echo "type ( sudo systemctl enable CAN_MQTT.service ) "
echo "then ( sudo systemctl start CAN_MQTT.service ) "

echo "also ( sudo reboot )"
