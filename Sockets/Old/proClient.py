# How to run: Open terminal, navigate to file and type: python3 proClient.py
# Enter username and start sending messages. Server terminal will continuously update information received
# Clients will update when sending data
"""
TODO    username must be taken from system (maybe MAC? In future AT-command for telephone no.).
        For now machine-id is used
TODO    Provide comments for all lines of code necessary
TODO    message sent must be stored json file
TODO    implement error-handling for checking if connection to server is lost
TODO    Implement connection-log. Log at first connect, connection loss and reconnect. Limit file-size/length
"""

import socket
import errno
import sys
import os
from time import sleep
import json

"""Header defined as 10 digits for client and server, allowing max message length from 0 to 999.999.999"""
HEADER_LENGTH = 10
IP = "127.0.0.1"
PORT = 8008

client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   # client-socket setup using IPv4 and TCP
client_socket.connect((IP, PORT))                                   # connect to specified IP and port
client_socket.setblocking(False)   # Ensures socket is nonblocking (might not be necessary due to defined headerlength?)

""" machine-id is used as ID sent to server. ID is stripped of trailing newline and assigned to osID variable.
    Encoded afterwards and sent to server. Server requires username/ID as first message when connecting"""
osID = str(os.popen("cat /etc/machine-id").read()).rstrip("\n")
gokartID = osID.encode('utf-8')
gokartID_header = f"{len(gokartID):<{HEADER_LENGTH}}".encode("utf-8")
client_socket.send(gokartID_header + gokartID)

connected = True        # Connection-status monitored through this variable
# TODO  insert print-to-connection-log here

while True:
    # Attempt to send data, otherwise reconnect
    try:
        # TODO  change to data-package file location, check for changes, and only send if data has been updated
        # Temporary blocking function. If omitted program sends data continuously, creating connection errors aswell
        userIn = input(f"Enter current:")
        data_package = {"voltage": 12.7, "lat": 5507901.25, "lon": 1002786.368, "current": userIn} # Test data

        # if-statement not necessary as-is, but might be when data-package and update-checking is implemented
        #if data_package:
        data_package = bytes(json.dumps(data_package), "utf-8")                 # json used to package data
        dp_header = bytes(f"{len(data_package):<{HEADER_LENGTH}}", "utf-8")     # defines data header length
        client_socket.send(dp_header + data_package)                            # sends header and data to server
    except socket.error:
        """ If an error occurs on the server-socket connection, this exception block runs and tries to reconnect"""
        connected = False
        client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   # Recreates client-socket
        print("Connection lost. Attempting reconnect...")
        while not connected:
            try:
                client_socket.connect((IP, PORT))
                client_socket.setblocking(False)
                client_socket.send(gokartID_header + gokartID)
                connected = True
                print("Reconnected successfully")
            except socket.error:
                print("Reconnection unsuccessful")
                sleep(2)
