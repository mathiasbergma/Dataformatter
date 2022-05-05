#
#!/bin/bash

sudo config-pin p2_17 gpio

echo out >> /sys/class/gpio/gpio65/direction

echo 0 >> /sys/class/gpio/gpio65/value

