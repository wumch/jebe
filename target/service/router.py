#coding:utf-8

import zmq
from config import config

router_addr = "tcp://*:%d" % config.router_port
dealer_addr = "tcp://%(host)s:%(port)d" % config.getRouter()

context = zmq.Context(config.iothreads)

router = context.socket(zmq.ROUTER)
router.bind(router_addr)

dealer = context.socket(zmq.DEALER)
dealer.bind(dealer_addr)

zmq.device(zmq.QUEUE, router, dealer)
