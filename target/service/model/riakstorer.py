#coding:utf-8

import os, sys
sys.path.append(os.path.dirname(os.path.dirname(__file__)))
import riak
from config import config
from utils.misc import *

class RiakStorer(object):

    buck = None      # make riak raise an error.
    backend = None
    R_VALUE = 1
    R_VALUE_UP = 2

    def __init__(self):
        self.riakClient = riak.RiakClient(**config.getRiak())
        self.bucket = self.riakClient.bucket(self.buck)
        self.bucket.set_property('backend', self.backend)

    def _genKey(self, url):
        return strenc(url)
