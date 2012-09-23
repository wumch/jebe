#!/usr/bin/env python

try:
    import psyco
    psyco.jit()
    from psyco.classes import *
except ImportError:
    pass

import sys
import gevent
from gevent.http import HTTPServer
from utils.natip import natip
from controler.wsgiwrap import WsgiApp

def run():
    addr = (natip, int(sys.argv[1]) if len(sys.argv) > 1 else 10020)
    backlog = 64
    app = WsgiApp()
    if gevent.version_info < (0, 13, 7):
        HTTPServer(app).serve_forever(socket_or_address=addr, backlog=backlog)
    else:
        HTTPServer(listener=addr, handle=app, backlog=backlog).serve_forever()

if __name__ == '__main__':
    greenlets = [gevent.spawn(run)]
    gevent.joinall(greenlets=greenlets)
