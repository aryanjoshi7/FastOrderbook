from flask import Flask, request
import socket
import json
import orderbook

app = Flask(__name__)

@app.route('/send')
def send():
    global order_id, o
    order_id += 1

    price = request.args.get("price")
    quantity = request.args.get("quantity")
    side = request.args.get("side")
    i = o.add_order(order_id, int(quantity), float(price), bool(int(side)))
    print("ADDED ORDER", i)
    a = dict(o.ask_map())
    b = dict(o.bid_map())
    asks  = []
    bids = []
    
    for k in a:
        asks.append({"price": k, "volume": a[k].volume})
    for k in b:
        bids.append({"price": k, "volume": b[k].volume})

    return {"asks": asks, "bids": bids}


o = orderbook.Orderbook()
order_id = 0
if __name__ == "__main__":
    app.run(host="0.0.0.0", debug=True)
