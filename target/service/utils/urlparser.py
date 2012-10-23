#coding:utf-8

from urlparse import urlparse

def unique(url):
    info = urlparse(url)

class UrlParser(object):

    _type = ('com', 'mobi', 'gov', 'edu', 'so', 'net', 'org', 'name', 'me', 'co', 'com', 'net', 'tel', 'info', 'biz', 'cc', 'tv')
    _area = ('cn', 'hk', 'jp', 'ko', 'ra', 'uk')

    def toPath(self, url):
        info = urlparse(url)
        scheme = self.parseScheme(info[0])
        domain = self.domainToPath(info[1])
        if scheme is None or domain is None:
            return None
        res = '/'.join(domain[::-1])
        path = self.parsePath(info[2])
        if path:
            if '.' in path[-1]:
                path.pop()
            res += '/' + '/'.join(path)
        return res

    def parse(self, url):
        return self.splitUrl(url)

    # TODO: delimiters ('' bwtween sub-domain and main-domain, '?' between path and query) are not so strict.
    def splitUrl(self, url):
        info = urlparse(url)
        scheme = self.parseScheme(info[0])
        domain = self.parseDomain(info[1])
        if scheme is None or domain is None:
            return None
        res = [scheme] + domain
        path = self.parsePath(info[2])
        if path:
            res += path
        query = self.parseQuery(info[4])
        if query:
            res += ['?']
            res += query
        return res

    def parsePath(self, path):
        return filter(len, [fragment.strip() for fragment in path.split(r'/')])

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

    def domainToPath(self, domain):
        info = (domain[:domain.find(':')] if ':' in domain else domain).split('.')
        parts = len(info)
        if parts == 1:
            return None
        elif parts == 2:
            return ['', info[0]]
        else:
            tldlen = 1
            last = info[-1]
            if last in self._type:
                subLen = parts - 2
            elif last in self._area:
                subLen = parts - 3
                tldlen = 2
            else:
                subLen = parts - 2
            return info[:subLen] + info[subLen:-tldlen]

    def parseQuery(self, query):
        return filter(len, query.split('&'))

    def parseScheme(self, scheme):
        return scheme if scheme in ('http', 'https') else None

def test(url):
    import os
    parser = UrlParser()
    print 'url: ', url, os.linesep,                        \
        'res: ', parser.toPath(url) or "this url is wrong-formated", os.linesep, os.linesep

if __name__ == '__main__':
    test('http://www.baidu.com')
    test('http://baidu.com')
    test('http:/www.baidu.com')
    test('http://www.baidu.com?fsdf=8998&fsdaf=fsdf')
    test('http://www.baidu.com/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://yundao.hi.baidu.com.cn/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://yundao.hi.baidu.com.xo/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://yundao.hi.baidu.cxx.xo/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://211.154.172.172/fasdfd/afsdfadf/file.html?fsdf=8998&fsdaf=fsdf')
