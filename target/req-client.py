#!/usr/bin/env python

import zmq, time, sys
from utils.natip import natip

context = zmq.Context()
sock = context.socket(zmq.REQ)
sock.connect("tcp://%s:10020" % natip)

message = ''.join(["a" for i in xrange(1, 10)])
time.clock()
for i in xrange(1, int(sys.argv[1])):
    sock.send(message)
    print(sock.recv())
print time.clock()
