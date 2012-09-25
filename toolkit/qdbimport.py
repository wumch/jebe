#coding:utf-8

import socket
from random import randint

class QDBImport(object):

    __default_port = 12580
    servers = (
        ('192.168.88.1', __default_port),
        ('192.168.88.2', __default_port),
    )
    action = 'import'

    def __init__(self):
        pass

    def prepare(self):
        server = self.servers[randint(0, len(self.servers) - 1)]
        print "will use %s:%d" % server
        self.sock = socket.socket(family=socket.AF_INET, type=socket.SOCK_STREAM)
        self.sock.connect(server)
        self.send(self.action)

    def importFile(self, filepath):
        try:
            self.prepare()
            fp = file(name=filepath, mode='r')
            for line in fp:
                self.send(line)
            else:
                print filepath, "done"
        except Exception, e:
            print "error occured: ", str(e.args)

    def send(self, data):
        return self._send(data=data)

    def _send(self, data):
        self.sock.send(data)

    def __del__(self):
        self.sock.close()

if __name__ == '__main__':
    import os, sys
    if len(sys.argv) < 2:
        print "usage: %s <file>" % sys.argv[0]
        sys.exit(1)
    QDBImport().importFile(filepath=sys.argv[1])
