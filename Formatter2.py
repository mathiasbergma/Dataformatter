import json
import time
import sys
from datetime import datetime

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


if len(sys.argv) != 5:
    print("Incorrect argument count")
    print("Usage: scriptname PathTofirstDataFile PathTosecondDataFile PathToOutputFile Delay[seconds]")
    sys.exit(2)
try:
    delay = float(sys.argv[4])
except ValueError:
    print("Delay must be a float or integer")
    sys.exit(2)
GPSpath = sys.argv[1]
CANpath = sys.argv[2]
Outputpath = sys.argv[3]

starttime = time.time()  # Get current system time
getdata(GPSpath)  # Retrieve data from GPS
getdata(CANpath)  # Retrieve data from CANBUS
data['time'] = str(datetime.now())  # Get data and time and append to datapacket
# Write to output file
with open(Outputpath, 'w') as f:
    f.write(json.dumps(data))
# Clear variable for next run
data = {}
stoptime = time.time()  # Get current system time
progtime = stoptime - starttime
print(progtime)
while True:
    # if time.time()-systime > 1:
    # systime = time.time()               # Get current system time
    getdata(GPSpath)  # Retrieve data from GPS
    getdata(CANpath)  # Retrieve data from CANBUS
    data['time'] = str(datetime.now())  # Get data and time and append to datapacket
    # Write to output file
    with open('senddata', 'w') as f:
        f.write(json.dumps(data))
    # Clear variable for next run
    data = {}
    time.sleep(delay - progtime)
