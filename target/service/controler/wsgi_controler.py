#coding:utf-8

class WsgiControler(object):

    def __init__(self, environ, start_response):
        self.env = environ
        self.start_response = start_response

    def handle(self):
        return ''
