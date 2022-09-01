import zmq
import random
import sys
import time
import json
import swigbuild.orderbook as orderbook
import threading

port = "12000"
context = zmq.Context()
socket = context.socket(zmq.PUB)
socket.bind("tcp://*:%s" % port)
o = orderbook.Orderbook()
order_id = 0

def send():
    global order_id, o, socket
    while True:
        a = list(o.ask_map())
        b = list(o.bid_map())
        asks  = []
        bids = []
        for k in a:
            asks.append({"price": k[0], "volume": k[1].volume})
        for k in b:
            bids.append({"price": k[0], "volume": k[1].volume})   
        print(asks, bids)
        message = json.dumps({"asks": asks, "bids": bids, "mark": o.mark})
        socket.send_string(message)
        time.sleep(0.5)
    
def listen():
    global order_id, o
    port2 = "10000"
    context2 = zmq.Context()
    socket2 = context2.socket(zmq.REP)
    socket2.bind("tcp://*:%s" % port2)
    msg = ""
    while True:
        order_id+=1
        msg = socket2.recv_json()
        socket2.send_string("ack")
        quantity, price, side = msg["quantity"], msg["price"], msg["side"]
        print(msg["quantity"], msg["price"], msg["side"])
        i = o.add_order(order_id, int(quantity), float(price), bool(int(side)))

first = threading.Thread(target = send)
second = threading.Thread(target = listen)

first.start()
second.start()