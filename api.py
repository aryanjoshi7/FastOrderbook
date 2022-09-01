from flask import Flask, request
import socket
import json

app = Flask(__name__)

@app.route('/send')
def send():
    clientSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    clientSocket.connect(("127.0.0.1", 8082))
    price = request.args.get("price")
    quantity = request.args.get("quantity")
    side = request.args.get("side")
    clientSocket.send("{} {} {}\n".format(quantity, price, side).encode())
    clientSocket.send("exit\n".encode())
    return "ok"

if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)