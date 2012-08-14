#coding:utf-8

import os, sys
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__)), 'target', 'service'))
from riak import RiakClient
from config import config

bucks = ('loc', 'mov', 'ads')
def search(*args):
    assert len(args) > 0, AssertionError('please specify at least one argument')
    buck = args[0] if args[0] in bucks else 'loc'
    term = ' '.join(map(lambda w: unicode(w, 'utf-8'), args[1:]))
    return riak.search(buck, u'loc:' + term)

riak = RiakClient(**config.getRiak())
print riak.bucket('loc').get('http://www.baidu.com/').get_data()
query = search('loc', '百度')

count = 0
for record in query.run():
    res = record.get().get_data()
    print 'res: ', res
    count += 1
    if count > 10:
        break
