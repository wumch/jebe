#!/usr/bin/env python
#coding:utf-8

import os, sys
import socket
from utils.natip import natip

sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock.bind((natip, 10010))
sock.listen(1)

def dump(data):
    if 'a' in data:
        print 'a is at:', data.find('a')
    print len(data)
    print(data)

(sock, addr) = sock.accept()
while True:
    message = sock.recv(100)
    dump(message)
