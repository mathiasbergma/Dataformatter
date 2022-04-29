#!/bin/bash

apt update && apt install -y vim curl make gcc build-essential libssl-dev git software-properties-common
add-apt-repository ppa:deadsnakes/ppa
apt update
apt install python3.9
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
make
make install
