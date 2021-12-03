# How to run: Open terminal, navigate to file and type: python3 proServer.py
# Server has now been started
# Check proClient.py to see how to connect as user
"""
TODO    Retool code to no longer work as messaging app, but one-directional data-server, only receiving data from
        clients and not sending/syncing clients
TODO    Provide comments for all lines of code necessary
TODO    Create connection log file where script prints connections and disconnections
"""

import socket
import select

""" Header length defined as 10 characters for client and server. 
    Allows for a max message length of 10 digits (from 0 to 999.999.999)"""
HEADER_LENGTH = 10
IP = "127.0.0.1"
PORT = 8008

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)      # Server-socket setup using IPv4 and TCP
# server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Allows reconnection w/o port being busy

server_socket.bind((IP, PORT))      # Binding server-socket
server_socket.listen()              # Setting server-socket to listen
sockets_list = [server_socket]      # Creates a list of sockets, server and clients included
gokarts = {}      # Creates a dict which will contain individual gokart data. Socket is key, gokart data is value


def receive_data(gokart_socket):
    """
    Function defining how to receive data from client
    1.  Start by receiving header data, specifying length of data to be received from client
    2.  If no header received in data-package, it means client disconnected, return false
    3.  Defines length of data according to data header, acting as a buffer the exact size of received data
        strip() removes empty spaces in header (I.e. "42        " becomes "42").
    4.  Receives data according to buffer size specified by data header
    5.  Returns a dict containing header and data
    6.  Returns false in except-clause if client unexpectedly/suddenly disconnects or breaks script
    """
    try:
        data_header = gokart_socket.recv(HEADER_LENGTH)
        if not len(data_header):
            return False
        message_length = int(data_header.decode("utf-8").strip())
        data = gokart_socket.recv(message_length)
        return {"header": data_header, "data": data}
    except:
        return False


while True:
    """ 
    select takes three parameters (read_list, write_list, exception_list), Creates a list of sockets to read in 
    data from, and sockets to check for exceptions. The write list is not used and therefore empty
    #select.select is the blocking function, preventing constant polling. Continues when a socket is notified 
    (ie. message received, client connects to server socket)
    """
    read_sockets, _, exception_sockets = select.select(sockets_list, [], sockets_list)

    for notified_socket in read_sockets:
        """ 
        If-statement activates if the notified socket is the server socket
        1.  Create two lists (for convenience) containing same info passed from .accept() (IP-address and Port) 
        2.  First data received is always user information. Received and assigned via receive_data function
        3.  if user is false, it means client disconnected. Jump to beginning of for-loop, into else-statement,
            into "if message is false", print client DC message
        4.  Append client socket to socket list, enabling notification via select.select for that socket
        5.  gokartID is a dict, which is assigned to the 'gokarts' dict where client_socket is the key, 
            and gokartID becomes value
        """
        if notified_socket == server_socket:
            client_socket, client_address = server_socket.accept()
            gokartID = receive_data(client_socket)
            if gokartID is False:
                continue
            sockets_list.append(client_socket)
            gokarts[client_socket] = gokartID
            print(f"Accepted new connection from {client_address[0]}:{client_address[1]} username:{gokartID['data'].decode('utf-8')}")

        """Else-statement activates if the notified socket is not the server-socket (ie. client socket sending message) """

        else:
            gokartData = receive_data(notified_socket)

            if gokartData is False:
                print(f"Closed connection from {gokarts[notified_socket]['data'].decode('utf-8')}")
                sockets_list.remove(notified_socket)
                del gokarts[notified_socket]
                continue

            user = gokarts[notified_socket]

            print(f"Received message from {user['data'].decode('utf-8')}: {gokartData['data'].decode('utf-8')}")

            for client_socket in gokarts:
                if client_socket != notified_socket:
                    client_socket.send(user['header'] + user['data'] + gokartData['header'] + gokartData['data'])
    for notified_socket in exception_sockets:
        sockets_list.remove(notified_socket)
        del gokarts[notified_socket]
