#coding:utf-8

import riak
from config import config
from utils.misc import *

class RiakStorer(object):

    buckId = None    # make key-error
    buck = None      # make riak raise an error.
    backend = None
    R_VALUE = 1
    R_VALUE_UP = 2

    def __init__(self):
        self.riakClient = riak.RiakClient(**config.getRiak())
        print 'self.__class__.__name__', self.__class__.__name__
        if self.buckId is not None:
            info = config.bucks[self.buckId]
            if self.buck is None:
                self.__class__.buck = info['buck']
            if self.backend is None:
                self.__class__.backend = info['backend']
        self.bucket = self.riakClient.bucket(self.buck)
        self.bucket.set_property('backend', self.backend)

    def _genKey(self, url):
        return strenc(url)
