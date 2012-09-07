#coding:utf-8

from urllib2 import urlopen
from model.riakstorer import RiakStorer
from config import config, logger

# fetch any type of corresponding data by supplied parameter.
class Fanin(RiakStorer):

    buck = 'loc'
    backend = 'hdd2'
    default_action = 'split'

    def __inti__(self, words=None, content=None, url=None):
        super(Fanin, self).__init__()
        self.words, self.content, self.url = words, config, url
        self.action = self.default_action
        self.slot = {}

    def getContent(self, **args):
        pass

    def getMarveTop(self, **args):
        pass

    def getMarveAll(self, **args):
        pass

    def getSplited(self, **args):
        pass

    def getCounted(self, **args):
        pass

    def __getattr__(self, item):
        return self.slot[item] if item in self.slot else getattr(self, 'get' + item.title())

    def _prepare(self, action=default_action):
        if self.words is None and self.content is None and self.url is not None:
            self.slot['splited'] = self._fetchSplitedContent()
        if self.content is not None:
            self.words = self._fromContent(action=action)
        else:
            raise ValueError('kid, both words and content of <%s> are None.' % self.__class__.__name__)

    def _fromContent(self, action=default_action):
        data = self.content.encode('utf-8') if isinstance(self.content, unicode) else self.content
        return config.jsoner.decode(urlopen(url=config.getTokenizer(), data=data, timeout=3).read())

    def _fetchSplitedContent(self):
        try:
            return self.bucket.get(self._genKey(self.url)).get_data()['words']
        except Exception, e:
            logger.error(('%s retrieve content by url:[%s] failed: ' % (type(e).__name__, self.url)) + str(e.args))
