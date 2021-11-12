import json
import time
from os import path
from datetime import datetime

GPSpath = 'gpsdata'
CANpath = 'candata'
data = {}

def stringparser(strs):
    for str_split in strs[0].split(','):
        try:
            data[str_split.split(':')[0]] = float(str_split.split(':')[1])
        except ValueError:
            data[str_split.split(':')[0]] = str_split.split(':')[1]
    return data
def getdata(datapath):
    with open(datapath, 'r') as f:
        text = f.read().splitlines()
    stringparser(text)
    return

with open('gpsdata', 'r') as f:
    text = f.read().splitlines()

GPSchange = path.getmtime(GPSpath)

stringparser(text)

with open('candata', 'r') as f:
    text = f.read().splitlines()

CANchange = path.getmtime(CANpath)

stringparser(text)
data['time'] = str(datetime.now())
print(data)
y = json.dumps(data)
print(y)
with open('senddata', 'w') as f:
    f.write(y)

lastGPSchange = GPSchange
lastCANchange = CANchange
data = {}
while True:
    if path.getmtime(GPSpath) != GPSchange:
        getdata(GPSpath)
    GPSchange = path.getmtime(GPSpath)

    if path.getmtime(CANpath) != CANchange:
        getdata(CANpath)
    CANchange = path.getmtime(CANpath)
    #print('lastGPSchange: ' + str(lastGPSchange-GPSchange))
    #print('lastCANchange: ' + str(lastCANchange-CANchange))
    if GPSchange != lastGPSchange and CANchange != lastCANchange:
        data['time'] = str(datetime.now())
        with open('senddata', 'w') as f:
            f.write(json.dumps(data))
        lastGPSchange = GPSchange
        lastCANchange = CANchange
        data = {}
    time.sleep(0.001)
