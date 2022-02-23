import paho.mqtt.client as mqtt
import random
import time
import json

mqttBroker = "test.mosquitto.org"
client = mqtt.Client("Gokart_2")
client.connect(mqttBroker)


def random_can():
    rand_can_id = random.randrange(100, 600)
    rand_can_message = hex(random.getrandbits(64))
    rand_can = [rand_can_id, rand_can_message]
    return rand_can


while True:
    data = random_can()
    data_out = json.dumps(data)
    client.publish("GOKART2", data_out)
    print(f"Just published {data} to topic DATA")
    time.sleep(0.5)
