#coding:utf-8

import zmq
from utils.natip import natip
from config import config

context = zmq.Context(config.iothreads)

router = context.socket(zmq.ROUTER)
router.bind("tcp://*:%d" % config.router_port)

dealer = context.socket(zmq.DEALER)
dealer.bind("tcp://%(host)s:%(port)d" % config.getRouter())

zmq.device(zmq.QUEUE, router, dealer)
