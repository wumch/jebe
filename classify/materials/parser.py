#coding:utf-8

import re
from urlparse import urlparse
from materials import *

class Parser(object):

    _type = ('com', 'mobi', 'gov', 'edu', 'so', 'net', 'org', 'name', 'me', 'co', 'com', 'net', 'tel', 'info', 'biz', 'cc', 'tv')
    _area = ('cn', 'hk', 'jp', 'ko', 'ra', 'uk')

    def __init__(self):
        self.title_spliter = re.compile(u'[\-— \|]')

    def parse(self, doc):
        info = urlparse(doc['url'])
        domain_info = self.parseDomain(info[1])
        paths = info[2].split('/', 2)
        path = paths[1] if len(paths) > 2 else ''
        return {
            URL : doc['url'],
            LOC : doc['url'][doc['url'].find('://')+3:83],
            TITLE : doc['title'],
            TITLE_SLICE:self.split_title(doc['title']),
            DOMAIN:info[1],
            MAIN_DOMAIN:domain_info[1],
            SUB_DOMAIN:domain_info[0],
            TOP_PATH:path,
        }

    def split_title(self, title):
        return self.title_spliter.split(title, 1)

    def parseDomain(self, domain):
        info = (domain[:domain.find(':')] if ':' in domain else domain).split('.')
        parts = len(info)
        if parts == 1:
            return None
        elif parts == 2:
            return ['', '.'.join(info)]
        else:
            last = info[-1]
            if last in self._type:
                subLen = parts - 2
            elif last in self._area:
                subLen = parts - 3
            else:
                subLen = parts - 2
            return ['.'.join(info[:subLen]), '.'.join(info[subLen:])]
