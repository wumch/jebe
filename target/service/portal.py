#!/usr/bin/env python

import gevent
from gevent.http import HTTPServer

try:
    import psyco
    psyco.jit()
    from psyco.classes import *
except ImportError:
    pass

from utils.natip import natip
from controler.wsgiwrap import WsgiApp

def run():
    addr = (natip, 8001)
    backlog = 512
    app = WsgiApp()
    if gevent.version_info < (0, 13, 8):
        HTTPServer(listener=addr, handle=app, backlog=backlog).serve_forever()
    else:
        HTTPServer(app).serve_forever(socket_or_address=addr, backlog=512)

if __name__ == '__main__':
    greenlets = [gevent.spawn(run)]
    gevent.joinall(greenlets=greenlets)
