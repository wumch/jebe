#coding:utf-8

from tads.ads import ads
from drivers.ftengine import FTEngine
from config import config
from tads.fallbackads import FallbackAd

class Adsupplier(object):

    cachedJsonEncoded = ads
    cached = {}

    # TODO: maybe a bug in multi threads?
    # thread 1 maybe get the response for thread 2 instead.
    _instance = None

    @classmethod
    def instance(cls):
        if cls._instance is None:
            cls._instance = cls()
        return cls._instance

    def __init__(self):
        self.ftengine = FTEngine.instance()

    def byMarvedWords(self, words, jsonEncoded=True, fallback=True):
        return self.byIds(self.ftengine.match(words=words), jsonEncoded=jsonEncoded, fallback=fallback)

    def byIds(self, adids, jsonEncoded=True, fallback=True):
        ads = filter(None, [self.byId(adid=adid, jsonEncoded=jsonEncoded) for adid in adids])
        if fallback and not ads:
            return [FallbackAd().getJSON() if jsonEncoded else FallbackAd().getAd()]
        return self.decorate(ads)

    def decorate(self, ads):
        if len(ads) == 1 and '119,' in ads[0]:
            ads[0] = FallbackAd().getAd()
        return ads

    def byId(self, adid, jsonEncoded=True):
        if jsonEncoded:
            return self.cachedJsonEncoded.get(adid)
        else:
            if not adid in self.cached:
                json = self.cachedJsonEncoded.get(adid)
                if json:
                    self.cached[adid] = config.jsoner.decode(json)
            return self.cached.get(adid)
