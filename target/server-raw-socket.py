#!/usr/bin/env python
#coding:utf-8

import os, sys
import socket
from natip import natip

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((natip, 10010))
sock.listen(1)

def dump(data):
    if 'a' in data:
        print 'a in pos:', data.find('a')
    print len(data)
    print(data)

(client, addr) = sock.accept()
while True:
    message = client.recv(100)
    dump(message)
