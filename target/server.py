#!/usr/bin/env python

import zmq

context = zmq.Context()
server = context.socket(zmq.REP)
#server.bind("tcp://127.0.0.1:10087")
server.bind("ipc://crawler")

while True:
    message = server.recv()
#    print(message)
    server.send(message)
