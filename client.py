import socket
import json
clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
clientSocket.connect(("127.0.0.1", 8080))
s = "1 101 1 ".encode()
e = "exit\n".encode()
clientSocket.send(s)
clientSocket.send(s)
clientSocket.send(e)
clientSocket.close()
