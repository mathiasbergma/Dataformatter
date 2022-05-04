#!/bin/bash

config-pin p1_26 can
config-pin p1_28 can
config-pin p2_25 can
config-pin p2_27 can

ip link set can0 up type can bitrate 125000
ip link set can1 up type can bitrate 125000
