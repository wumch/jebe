#coding:utf-8

from config import config, logger
from riakstorer import RiakStorer
from utils.MarveWords import MarveWords
from pagestorer import PageStorer

# match by words
class Matcher(RiakStorer):

    buckId = 'ads'
    field = 'words'
    field = u'words'

    max_ads = 1

    def __init__(self):
        super(Matcher, self).__init__()

    def match(self, words=None, content=None, loc=None, buck=config.bucks[buckId]['buck'], field=field):
        if words is None and content is None and loc is not None:
            splited_content = self._fetchSplitedContent(loc)
            if splited_content is None:
                return []
            ws = MarveWords(content=splited_content).top()
        else:
             ws = MarveWords(words=words, content=content).top()
        return self.search(ws, buck=buck, field=field)

    def search(self, words, buck=config.bucks[buckId]['buck'], field=field):
#        return [record.get().get_data() for record in self._search(words=words, buck=buck, field=field).run()]
        res = []
        count = 0
        for record in self._search(words=words, buck=buck, field=field).run():
            count += 1
            if count > self.max_ads:
                break
            res.append(record.get().get_data())
        return res

    def _search(self, words, buck, field):
        term = field + u':' + u' OR '.join(map(lambda w: w if isinstance(w, unicode) else unicode(w, 'utf-8'), words))
        return self.riakClient.search(buck, term)

    def _fetchSplitedContent(self, url):
        try:
            obj = PageStorer.instance().bucket.get(self._genKey(url))
            return obj.get_data()['words'] if obj and obj.exists() else None
        except Exception, e:
            logger.error(('%s retrieve content by url:[%s] failed: ' % (type(e).__name__, url)) + str(e.args))
