import socket


# Assign the socket, host and port for later use
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
host = '192.168.0.222'
port = 55556

# Set the time after which an exception is thrown after failure to perform an operation
s.settimeout(10)

try: 
    # Connect to the server (AKA the GPS unit)
    s.connect((host, port))

    # Loop and read messages continuously
    while True:
        msg = s.recv(1024)
        print "Data received: ", msg
except socket.error:
    print "There was a socket error in the connect then read loop"

