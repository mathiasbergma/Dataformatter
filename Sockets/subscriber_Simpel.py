import paho.mqtt.client as mqtt
import json

# when message is received on subscribed items, decode to json, then decode json to python object
def on_message(client, userdata, message):
    mqtt_to_json_decode = message.payload.decode("utf-8")
    json_to_pyobject = json.loads(mqtt_to_json_decode)
    print(f"Topic: {message.topic} Message: {json_to_pyobject}")


mqttBroker = "test.mosquitto.org"
client = mqtt.Client("Server")
client.connect(mqttBroker)
client.subscribe([("GOKART1", 1), ("GOKART2", 1)])
client.on_message = on_message
# Starts forever-loop (http://www.steves-internet-guide.com/loop-python-mqtt-client/)
print("Starting forever-loop")
client.loop_forever()  # must be at end of script, since it blocks. handles reconnects automatically
