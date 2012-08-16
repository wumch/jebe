#coding:utf-8

from config import config, logger
from riakstorer import RiakStorer
from utils.MarveWords import MarveWords
from utils.misc import *
from model.pagestorer import PageStorer

# match by words
class Matcher(RiakStorer):

    buckId = 'ads'
    field = 'words'
    field = u'words'

    def __init__(self):
        super(Matcher, self).__init__()

    def match(self, words=None, content=None, loc=None, buck=config.bucks[buckId]['buck'], field=field):
        if words is None and content is None and loc is not None:
            splited_content = self._fetchSplitedContent(loc)
            if not splited_content:
                return
            ws = MarveWords(content=splited_content).top()
        else:
             ws = MarveWords(words=words, content=content).top()
        ws[0] = u'坦克世界'
        print 'words: ',
        export(ws)
        print 'buck:', buck, ', field:', field
        return self.search(ws, buck=buck, field=field)

    def search(self, words, buck=config.bucks[buckId]['buck'], field=field):
        res = [record.get().get_data() for record in self._search(words=words, buck=buck, field=field).run()]
        print 'ads res: ',
        export(res)
        return res

    def _search(self, words, buck, field):
        term = field + u':' + u'+'.join(map(lambda w: w if isinstance(w, unicode) else unicode(w, 'utf-8'), words))
        print 'term: ', term
        return self.riakClient.search(buck, term)

    def _fetchSplitedContent(self, url):
        try:
            obj = PageStorer.instance().bucket.get(self._genKey(url))
            print 'exists: ', obj.exists()
            export(obj.get_data())
            return obj.get_data()['words'] if obj and obj.exists() else None
        except Exception, e:
            logger.error(('%s retrieve content by url:[%s] failed: ' % (type(e).__name__, url)) + str(e.args))
