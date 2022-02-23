import random
import time
import json
import paho.mqtt.client as paho
from paho import mqtt

# setting callbacks for different events to see if it works, print the message etc.
def on_connect(client, userdata, flags, rc, properties=None):
    print("CONNACK received with code %s." % rc)

# with this callback you can see if your publish was successful
def on_publish(client, userdata, mid, properties=None):
    print("mid: " + str(mid))

# print which topic was subscribed to
def on_subscribe(client, userdata, mid, granted_qos, properties=None):
    print("Subscribed: " + str(mid) + " " + str(granted_qos))

# print message, useful for checking if it was successful
def on_message(client, userdata, msg):
    print(msg.topic + " " + str(msg.qos) + " " + str(msg.payload))


client = paho.Client(client_id="", userdata=None, protocol=paho.MQTTv5)
client.on_connect = on_connect

# enable TLS for secure connection
client.tls_set(tls_version=mqtt.client.ssl.PROTOCOL_TLS)
# set username and password
client.username_pw_set("gokart1", "Gokart123")
# connect to HiveMQ Cloud on port 8883 (default for MQTT)
client.connect("a9db320b026b492dbcf3fe60394f02ef.s1.eu.hivemq.cloud", 8883)


def random_can():
    rand_can_id = random.randrange(100, 600)
    rand_can_message = hex(random.getrandbits(64))
    rand_can = [rand_can_id, rand_can_message]
    return rand_can


while True:
    data = random_can()
    data_out = json.dumps(data)
    client.publish("gokart/1", data_out)
    print(f"Just published {data} to topic DATA")
    time.sleep(1)
