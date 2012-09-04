#coding:utf-8

from random import randint
from config import config

fallback_ads = {
    201 : {
        'id' : 201,
        'link' : 'http://huo360.com?gngo=t',
        'text' : (
            u'最想最劲八卦美剧资讯',
            u'复仇者联盟将拍电视版',
            u'美国名人最新收入排名',
            u'2012年美剧播出时间表',
            u'热门美剧追看收视排行',
        ),
    },
}

class FallbackAd(object):

    def __init__(self):
        self.aid = 201

    def getAd(self):
        return self._get()

    def getJSON(self):
        return config.jsoner.encode(self._get())

    def _get(self):
        ad = fallback_ads[self.aid]
        turn = randint(0, len(ad['text']) - 1)
        return {
            'id' : ad['id'],
            'link' : ad['link'] + str(turn + 1),
            'text' : ad['text'][turn]
        }
