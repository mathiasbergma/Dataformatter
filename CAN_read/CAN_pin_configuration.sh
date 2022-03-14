#!/bin/bash
config-pin p1_26 can
config-pin p1_28 can
sudo ip link set can0 up type can bitrate 50000
sudo ifconfig can0 up
