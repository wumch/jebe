#!/usr/bin/env python

import zmq
from natip import natip

context = zmq.Context()
server = context.socket(zmq.REP)
server.bind("tcp://%s:10010" % natip)
#server.bind("ipc://crawler")

while True:
    message = server.recv()
    print(message)
    server.send(message)

