#!/bin/bash

apt update && apt install -y vim curl make gcc build-essential libssl-dev git software-properties-common libnss3-dev zlib1g-dev libgdbm-dev libncurses5-dev libssl-dev libffi-dev libreadline-dev libsqlite3-dev libbz2-dev
cd
mkdir Downloads
cd Downloads
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
make
make install
cd
git clone https://github.com/mathiasbergma/Gokart_CAN_API.git
python3 -m pip install can
python3 -m pip install python_can
python3 -m pip install paho_mqtt
~/Gokart_CAN_API/3G_sakis3g_mod/copy_services.sh
systemctl enable connection.service
systemctl start connection.service

