#!/usr/bin/env python
#coding:utf-8

import os, sys
import socket
from utils.natip import natip
import base64

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((natip, 10010))
sock.listen(1)

def dump(data):
    if 'a' in data:
        print 'a is at:', data.find('a')
    print len(data)
    print base64.b64encode(data)
#    print(data)

while True:
    (client_sock, addr) = sock.accept()
    message = client_sock.recv(100)
    dump(message)
