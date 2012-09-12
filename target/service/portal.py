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

if __name__ == '__main__':
    greenlets = [gevent.spawn(HTTPServer((natip, 8001), handle=WsgiApp()).serve_forever)]
    gevent.joinall(greenlets=greenlets)
