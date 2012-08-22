#coding:utf-8

from urllib2 import urlopen
from config import config, logger

class Tokenizer(object):

    def __init__(self):
        pass

    def marve(self, content):
        return config.jsoner.decode(self.request(data=content, action='marve'))

    def count(self, content):
        return config.jsoner.decode(self.request(data=content, action='count'))

    def split(self, content):
        return self.request(data=content, action='split')

    def request(self, data, action):
        try:
            return urlopen(config.getTokenizer(action), data, timeout=3).read()
        except Exception, e:
            logger.error(('kid, request to tokenizer/split with len(content)=%d failed: ' % len(data)) + str(e.args))
