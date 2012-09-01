#!/usr/bin/python
#coding:utf-8

from utils.natip import wanip, natip
from SocketServer import (ThreadingTCPServer as TCP, StreamRequestHandler as SRH)

class RequestHandler(SRH):
    def handle(self):
        self.wfile.write('<cross-domain-policy><allow-access-from domain="*" to-ports="*"/></cross-domain-policy>')

def start_server():
    listen = (wanip or natip, 843)
    tcpServ = TCP(listen, RequestHandler)
    print 'listen on %s:%d' % listen
    tcpServ.serve_forever()

if __name__ == '__main__':
    start_server()
