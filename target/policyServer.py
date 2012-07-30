#!/usr/bin/python
#encoding:utf-8
# Filename:policy_file_flash.py
''' flash policy file '''

import os
from SocketServer import (TCPServer as TCP, StreamRequestHandler as SRH)

class MyRequestHandler(SRH):
    '''MyRequestHandler'''

    def handle(self):
        print '...connected from:', self.client_address
        print self.rfile.read(23)
        self.wfile.write('<cross-domain-policy>\
                            <allow-access-from domain="*" to-ports="*"/>\
                          </cross-domain-policy>\
                         ')
        print 'send xml ok'

class PolicyFileFlash():
    '''PolicyFileFlash'''

    def __init__(self):
        tcpServ = TCP(("10.10.11.163", 843), MyRequestHandler)
        print 'waiting for connection...'
        # 死循环
        tcpServ.serve_forever()

#!/usr/bin/python
# Filename:main.py
'''main interface'''

def start_server():
    policy_file = PolicyFileFlash()

# begin
if __name__ == '__main__':
    start_server()

