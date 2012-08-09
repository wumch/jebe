#coding:utf-8

from urlparse import urlparse

class UrlParser(object):

    _type = ('com', 'mobi', 'gov', 'edu', 'so', 'net', 'org', 'name', 'me', 'co', 'com', 'net', 'tel', 'info', 'biz', 'cc', 'tv')
    _area = ('cn', 'hk', 'jp', 'ko', 'ra')

    def parse(self, url):
        return self.splitUrl(url)

    def splitUrl(self, url):
        info = urlparse(url)
        scheme = self.parseScheme(info[0])
        domain = self.parseDomain(info[1])
        if scheme is None or domain is None:
            return None
        res = scheme + ' ' + ' '.join(domain)
        path = self.parsePath(info[2])
        if path:
            res += ' ' + ' '.join(path)
        query = self.parseQuery(info[4])
        if query:
            res += ' ' + ' '.join(query)
        return res

    def parsePath(self, path):
        info = filter(len, [fragment.strip() for fragment in path._split(r'/')])
        return [(r'/' * i + info[i]) for i in range(0, len(info))]

    def parseDomain(self, domain):
        info = (domain[:domain.find(':')] if ':' in domain else domain)._split('.')
        parts = len(info)
        if parts == 1:
            return None
        elif parts == 2:
            return info[:1]
        else:
            last = info[-1]
            if last in self._type:
                subLen = parts - 2
            elif last in self._area:
                subLen = parts - 3
            else:
                subLen = parts - 2
            return '.'.join(info[:subLen]), '.'.join(info[subLen:])

    def parseQuery(self, query):
        return filter(len, query._split('&'))

    def parseScheme(self, scheme):
        return scheme if scheme in ('http', 'https') else None

def test(url):
    import os
    parser = UrlParser()
    print 'url:  ', url, os.linesep,                        \
        'info: ', parser.parse(url), os.linesep, os.linesep

if __name__ == '__main__':
    test('http://www.baidu.com')
    test('http:/www.baidu.com')
    test('http://www.baidu.com?fsdf=8998&fsdaf=fsdf')
    test('http://www.baidu.com/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://yundao.hi.baidu.com.cn/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://yundao.hi.baidu.com.xo/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://yundao.hi.baidu.cxx.xo/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
    test('http://211.154.172.172/fasdfd/afsdfadf/fasdf?fsdf=8998&fsdaf=fsdf')
