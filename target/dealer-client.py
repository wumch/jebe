#!/usr/bin/env python

import zmq, time, sys

context = zmq.Context()

client = context.socket(zmq.REP)
client.connect("tcp://127.0.0.1:10011")

message = ''.join(["a" for i in xrange(1, 10)])
time.clock()
for i in xrange(1, int(sys.argv[1])):
    print(client.recv())
    client.send(message)
print time.clock()

