#coding:utf-8

import zmq
from config import config

router_addr = "tcp://*:%d" % config.router_port
dealer_addr = "tcp://%(host)s:%(port)d" % config.getRouter()
print "router:%s, dealer:%s" % (router_addr, dealer_addr)

context = zmq.Context(config.iothreads)

