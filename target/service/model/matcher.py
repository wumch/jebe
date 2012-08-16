#coding:utf-8

from config import config, DEBUG, logger
from riakstorer import RiakStorer
from utils.MarveWords import MarveWords

# match by words
class Matcher(RiakStorer):

    buckId = 'ads'
    if DEBUG:
        backId = 'asus'
    buck, field = config.bucks[buckId]['buck'], config.bucks[buckId]['field']
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
        term = field + u':' + u' '.join(map(lambda w: w if isinstance(w, unicode) else unicode(w, 'utf-8'), words))
        return self.riakClient.search(buck, term)

    def _fetchSplitedContent(self, url):
        try:
            return self.bucket.get(self._genKey(url)).get_data()['words']
        except Exception, e:
            logger.error(('%s retrieve content by url:[%s] failed: ' % (type(e).__name__, url)) + str(e.args))
