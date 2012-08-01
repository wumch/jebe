#coding:utf-8

import zmq
from utils.natip import natip

context = zmq.Context(1)
context.sockopts(zmq.HWM, 50000)

router = context.socket(zmq.ROUTER)
router.bind("tcp://%s:10010" % natip)

dealer = context.socket(zmq.DEALER)
dealer.bind("tcp://%s:10011" % natip)

zmq.device(zmq.QUEUE, router, dealer)
