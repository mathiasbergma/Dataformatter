import socket
import select

# Header length defined as 10 characters for client and server
HEADER_LENGTH = 10
IP = "127.0.0.1"
PORT = 8008

# Server-socket setup using IPv4 and TCP
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # Allows reconnection w/o port being busy

# Binding server-socket
server_socket.bind((IP, PORT))

# Setting server-socket to listen
server_socket.listen()

# Creates a list of sockets, server and clients included
sockets_list = [server_socket]

# Creates a dictionary which will contain individual gokart data. Socket is key, gokart data is value
clients = {}


# Function defining how to receive data from client
def receive_data(gokart_socket):
    try:
        # Start by receiving header data, specifying length of data received from client
        data_header = gokart_socket.recv(HEADER_LENGTH)
        # If no header received in data-package, it means client disconnected
        if not len(data_header):
            return False
        # Defines length of data according to data header, acting as a buffer the exact size of received data
        # strip() removes empty spaces in header (I.e. "26        " becomes "26").
        message_length = int(data_header.decode("utf-8").strip())
        # Receives data according to buffer size specified by data header
        data = gokart_socket.recv(message_length)
        # Returns a dict containing header and data
        return {"header": data_header, "data": data}
    # Returns false if client unexpectedly/suddenly disconnects or breaks script
    except:
        return False


while True:
    # select takes three parameters (read_list, write_list, exception_list)
    # Creates a list of sockets to read in data from, and sockets to check for exceptions.
    # The write list is not used and therefore empty
    read_sockets, _, exception_sockets = select.select(sockets_list, [], sockets_list)

    for notified_socket in read_sockets:
        if notified_socket == server_socket:
            client_socket, client_address = server_socket.accept()

            user = receive_data(client_socket)
            if user is False:
                continue
            sockets_list.append(client_socket)

            clients[client_socket] = user

            print(f"Accepted new connection from {client_address[0]}:{client_address[1]} username:{user['data'].decode('utf-8')}")

        else:
            message = receive_data(notified_socket)

            if message is False:
                print(f"Closed connection from {clients[notified_socket]['data'].decode('utf-8')}")
                sockets_list.remove(notified_socket)
                del clients[notified_socket]
                continue

            user = clients[notified_socket]

            print(f"Received message from {user['data'].decode('utf-8')}: {message['data'].decode('utf-8')}")

            for client_socket in clients:
                if client_socket != notified_socket:
                    client_socket.send(user['header'] + user['data'] + message['header'] + message['data'])
    for notified_socket in exception_sockets:
        sockets_list.remove(notified_socket)
        del clients[notified_socket]