#!/bin/bash

./pins_setup.sh
sh -c './home/debian/Gokart_CAN_API/Onboard_CAN_MQTT_Client/CANsniff.py & ./home/debian/Gokart_CAN_API/Power_off/Pocket_shutdown'
