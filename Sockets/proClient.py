# How to run: Open terminal, navigate to file and type: python3 proClient.py
# Enter username and start sending messages. Server terminal will continuously update information received
# Clients will update when sending data
"""
TODO    username must be taken from system (maybe MAC? In future AT-command for telephone no.).
        For now machine-id is used
TODO    Provide comments for all lines of code necessary
TODO    message sent must be stored json file
TODO    implement error-handling for checking if connection to server is lost
"""

import socket
import errno
import sys
import os

"""Header defined as 10 digits for client and server, allowing max message length from 0 to 999.999.999"""
HEADER_LENGTH = 10
IP = "127.0.0.1"
PORT = 8008

# machine-id is used as ID sent to server. ID is stripped of trailing newline and assigned to osID variable
osID = str(os.popen("cat /etc/machine-id").read()).rstrip("\n")
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)   # client-socket setup using IPv4 and TCP
client_socket.connect((IP, PORT))                                   # connect to specified IP and port
client_socket.setblocking(False)    # Ensures

gokartID = osID.encode('utf-8')
username_header = f"{len(gokartID):<{HEADER_LENGTH}}".encode("utf-8")
client_socket.send(username_header + gokartID)

while True:
    message = input(f"{osID} > ")

    if message:
        message = message.encode('utf-8')
        message_header = f"{len(message):<{HEADER_LENGTH}}".encode('utf-8')
        client_socket.send(message_header + message)
"""   
    try:
        while True:
            # Receive things
            username_header = client_socket.recv(HEADER_LENGTH)
            if not len(username_header):
                print("Connection closed by the server")
                sys.exit()

            username_length = int(username_header.decode('utf-8').strip())
            username = client_socket.recv(username_length).decode('utf-8')

            message_header = client_socket.recv(HEADER_LENGTH)
            message_length = int(message_header.decode('utf-8').strip())
            message = client_socket.recv(message_length).decode('utf-8')

            print(f"{username} > {message}")

    except IOError as e:
        if e.errno != errno.EAGAIN and e.errno != errno.EWOULDBLOCK:
            print("Reading error", str(e))
            sys.exit()
        continue

    except Exception as e:
        print("General error", str(e))
        sys.exit()
        """
