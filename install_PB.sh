#!/bin/bash

apt update && apt install -y \
    vim \
    curl \
    make \
    gcc \
    build-essential \
    libssl-dev \
    git \
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c
RUN make
RUN make install
