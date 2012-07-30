#coding:utf-8

import zmq

context = zmq.Context(2)

router = context.socket(zmq.ROUTER)
router.bind("tcp://*:10010")

dealer = context.socket(zmq.DEALER)
dealer.bind("tcp://127.0.0.1:10011")

zmq.device(zmq.QUEUE, router, dealer)

