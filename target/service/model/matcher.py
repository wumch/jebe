#coding:utf-8

from config import config, DEBUG, logger
from riakstorer import RiakStorer
from utils.MarveWords import MarveWords
from utils.misc import strenc

# match by words
class Matcher(RiakStorer):

    buck = 'loc'
    backend = 'hdd2'
    if DEBUG:
        backend = 'leveldb'
    field = u'words'

    def __init__(self):
        super(Matcher, self).__init__()

    def match(self, words=None, content=None, loc=None, buck=buck, field=field):
        if words is None and content is None and loc is not None:
            splited_content = self._fetchSplitedContent(loc)
            ws = MarveWords(content=splited_content).top()
        else:
             ws = MarveWords(words=words, content=content).top()
        return self.search(ws, buck=buck, field=field)

    def search(self, words, buck=buck, field=field):
        return [record.get().get_data() for record in self._search(words=words, buck=buck, field=field).run()]

    def _search(self, words, buck, field):
        term = field + u':' + u' '.join(map(lambda w: unicode(w, 'utf-8'), words))
        return self.riakClient.search(buck, term)

    def _fetchSplitedContent(self, url):
        try:
            return config.jsonDecoder.decode(self.bucket.get(strenc(url)))['words']
        except Exception, e:
            logger.error(('retrieve content by url:[%s] failed: ' % url) + str(e.args))