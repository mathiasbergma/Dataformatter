#!/usr/bin/env python3
"""
Author:         Team1/Anders M. Andersen
Version:        0.2
Description:    Reads from CAN-bus on a beagleboard and publishes CAN-ID and CAN-message to a topic using MQTT.
                TLS implemented for improved security.
                How to setup:
                    Put TLS certificates in correct folder on embedded platform
                    Ensure certificate paths are correct (var: ca, cert, key)
                    Enter public IP address and port for server (var: server, port)
                    Set what MQTT topic to publish to. Standard is gokart/#/can, where # indicates gokart no. and can
                        is the subfolder containing can-messages
                    Set id, which is the name given the gokart
                    If necessary change CAN socket and bitrate according to specs used on gokart
"""

import can
import paho.mqtt.client as paho
from paho import mqtt
import json
import binascii
from time import sleep as sleep

# TLS variables - TODO use config file as reference to certificates. Ensures script works when cert. names change
ca = "/home/debian/Gokart_CAN_API/client_certs/ca.crt"
cert = "/home/debian/Gokart_CAN_API/client_certs/client.crt"
key = "/home/debian/Gokart_CAN_API/client_certs/client.key"
tls_version = mqtt.client.ssl.PROTOCOL_TLSv1_2

# MQTT variables:
server = "ec2-18-196-165-240.eu-central-1.compute.amazonaws.com"
port = 8883
mqtt_version = paho.MQTTv5
id = "gokart1"
topic = "gokart/1/can"
connected = False   # Do not change. Ensures startup without initial internet connection

# CAN variables
bustype = 'socketcan'
channel = 'can0'        # Change according to used CAN-bus in final implementation
bitrate = 125000        # Change to bitrate used in final implementation


# Setting callback for on_connect. Used to set global var 'connected' if false.
def on_connect(client, userdata, flags, rc, properties=None):
    global connected
    if not connected:
        print("Connected to broker")
        connected = True


# Setting callback for on_disconnect. Used to set global var 'connected' if true.
# Only used to ensure D/C message is printed once
def on_disconnect(client, userdata, flags, rc, properties=None):
    global connected
    if connected:
        print("Disconnected from broker")
        connected = False


client = paho.Client(client_id=id, userdata=None, protocol=mqtt_version)

# Enable TLS for secure connection
client.tls_set(ca_certs=ca, certfile=cert, keyfile=key, tls_version=tls_version)
client.tls_insecure_set(True) # Makes connection insecure. Must not be implemented in final product

# Setting callbacks
client.on_connect = on_connect
client.on_disconnect = on_disconnect

if __name__ == "__main__":
    # Try to connect to AWS Cloud on port 8883 (default for MQTT). If no internet connection on script startup
    # sleep for 1 sec and try to reconnect. 'connected' is set in on_connect/disconnect callbacks
    while not connected:
        try:
            client.connect(server, port)
            client.loop_start()
        except:
            sleep(1)
    # Reads from CAN-bus, create JSON to be sent, publishes to topic
    with can.interface.Bus(bustype=bustype, channel=channel, bitrate=bitrate) as bus:
        while True:
            msg = bus.recv() # Blocking read. Will wait for CAN-message before continuing
            if msg is not None:
                # Using hexlify to quickly convert msg.data bytearray to hex-string
                CAN_package = [msg.timestamp, msg.arbitration_id, binascii.hexlify(msg.data).decode('ascii')]
                data_out = json.dumps(CAN_package)
                client.publish(topic, data_out)
