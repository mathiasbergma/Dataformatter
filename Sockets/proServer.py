# How to run: Open terminal, navigate to file and type: python3 proServer.py
# Server has now been started
# Check proClient.py to see how to connect as user
"""
TODO    Create connection log file where script prints connections and disconnections
TODO    setup connection across local network
TODO    setup connection across internet
"""

import socket
import select

"""Header defined as 10 digits for client and server, allowing max message length from 0 to 999.999.999"""
HEADER_LENGTH = 10
IP = "127.0.0.1"
PORT = 8008

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)       # Server-socket setup using IPv4 and TCP
# server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)   # Allows reconnection w/o port being busy

server_socket.bind((IP, PORT))  # Binding server-socket
server_socket.listen()          # Setting server-socket to listen
sockets_list = [server_socket]  # Creates a list of sockets, server and clients included
gokarts = {}                    # Creates dict which will contain individual gokart data. Socket=key, gokart data=value


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
    select.select is the blocking function, preventing constant polling. Continues when a socket is notified 
    (ie. message received, client connects to server socket)
    """
    read_sockets, _, exception_sockets = select.select(sockets_list, [], sockets_list)

    """ for loop handles the notified sockets put in the read_sockets list"""
    for notified_socket in read_sockets:
        """ 
        If-statement activates if the notified socket is the server socket
        1.  Create two lists (for convenience) containing same info passed from .accept() (IP-address and Port)
            Note: .accept() returns a tuple containing IP and Port of accepted socket 
        2.  First data received is always user information. Received and assigned via receive_data function
        3.  if user is false, it means client disconnected. Jump to beginning of for-loop, into else-statement,
            into "if message is false", print client DC message
        4.  Append client socket to socket list, enabling notification via select.select for that socket
        5.  gokartID is a dict, which is assigned to the 'gokarts' dict where client_socket is the key, 
            and gokartID becomes value
        6.  prints accept message to console (note TODO)
        Else-statement activates if the notified socket is not the server-socket (ie. client socket sending message)
        1.  Gokart data is received via receive_data function
        2.  If receive_data function returns false, it means connection to client has been closed
            prints DC message, removes notified socket from sockets_list and deletes the notified_socket object 
            placed in the gokarts dict. Finally calls continue, returning to start of for-loop
        3.  gokartID variable is assigned the value stored in gokarts dict for the notified socket
        4.  Prints receive-message containing gokartID and gokartData
        """
        if notified_socket == server_socket:
            client_socket, client_addr = server_socket.accept()
            gokartID = receive_data(client_socket)
            if gokartID is False:
                continue
            sockets_list.append(client_socket)
            gokarts[client_socket] = gokartID
            print(f"Accepted new connection from {client_addr[0]}:{client_addr[1]} "
                  f"username:{gokartID['data'].decode('utf-8')}")
        else:
            gokartData = receive_data(notified_socket)
            if gokartData is False:
                print(f"Closed connection from {gokarts[notified_socket]['data'].decode('utf-8')}")
                sockets_list.remove(notified_socket)
                del gokarts[notified_socket]
                continue
            gokartID = gokarts[notified_socket]
            print(f"Received message from {gokartID['data'].decode('utf-8')}: {gokartData['data'].decode('utf-8')}")

    """ for-loop handles notified sockets put in the exception_sockets list. Ensures sockets with exceptions are 
        removed from the sockets_list and deletes the notified exception socket from the gokarts dict."""
    for notified_socket in exception_sockets:
        sockets_list.remove(notified_socket)
        del gokarts[notified_socket]
