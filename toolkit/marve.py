#!/usr/bin/env python
#coding:utf-8

import os, sys
from urllib2 import urlopen
sys.path.insert(os.path.join(os.path.dirname(os.path.dirname(__file__)), 'target', 'utils'))
from util.config import config


def crawl(url):
    content = urlopen(url, timeout=20).read()
