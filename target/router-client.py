#!/usr/bin/env python

import zmq, time, sys

context = zmq.Context()

client = context.socket(zmq.REQ)
client.connect("tcp://127.0.0.1:10010")

message = ''.join(["a" for i in xrange(1, 10)])
message2 = ''.join(["b" for i in xrange(1, 19)])
time.clock()
for i in xrange(1, int(sys.argv[1])):
    client.send(message)
    client.recv()
print time.clock()

