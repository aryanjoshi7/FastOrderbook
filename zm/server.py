import zmq.green as zmq
import json
import gevent
from flask_sockets import Sockets, Rule
from flask import Flask, render_template, request
import logging
from gevent import monkey

monkey.patch_all()

app = Flask(__name__)
logging.basicConfig(level=logging.INFO)
logger = logging.getLogger(__name__)

sockets = Sockets(app)
context = zmq.Context()

ZMQ_LISTENING_PORT = 12000

@app.route('/send')
def send():
    socket = context.socket(zmq.REQ)
    socket.connect('tcp://localhost:10000')
    socket.send_json({"price": request.args.get("price"), "quantity": request.args.get("quantity"), "side": request.args.get("side")})
    return "sent"

@app.route('/')
def index():
    logger.info('Rendering index page')
    return render_template('index.html')

@sockets.route('/zeromq', websocket=True)
def send_data(ws):
    logger.info('Got a websocket connection, sending up data from zmq')
    socket = context.socket(zmq.SUB)
    socket.connect('tcp://localhost:{PORT}'.format(PORT=ZMQ_LISTENING_PORT))
    socket.setsockopt_string(zmq.SUBSCRIBE, "")
    poller = zmq.Poller()
    poller.register(socket, zmq.POLLIN)
    gevent.sleep()
    received = 0
    while True:
        received += 1
        # socks = dict(poller.poll())
        # if socket in socks and socks[socket] == zmq.POLLIN:
        data = socket.recv_json()
        logger.info(str(received)+str(data))
        ws.send(json.dumps(data))
        gevent.sleep()

sockets.url_map.add(Rule('/zeromq', endpoint=send_data, websocket=True))

if __name__ == '__main__':
    logger.info('Launching web server')
    from gevent import pywsgi
    from geventwebsocket.handler import WebSocketHandler
    server = pywsgi.WSGIServer(('', 25000), app, handler_class=WebSocketHandler)
    logger.info('Starting serving')
    server.serve_forever()