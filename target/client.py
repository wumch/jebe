#!/usr/bin/env python

import zmq, time, sys

context = zmq.Context()

client = context.socket(zmq.REQ)
client2 = context.socket(zmq.REQ)
#client.connect("tcp://127.0.0.1:10087")
client.connect("ipc://crawler")
client2.connect("ipc://crawler")

message = ''.join(["a" for i in xrange(1, 10)])
message2 = ''.join(["b" for i in xrange(1, 19)])
time.clock()
for i in xrange(1, int(sys.argv[1])):
    client.send(message)
    client2.send(message2)
    print(client.recv())
    print(client2.recv())
#    print(client.recv())
print time.clock()

