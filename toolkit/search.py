#coding:utf-8

import os, sys
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__)), 'target', 'service'))
from riak import RiakClient
from config import config
from pprint import pprint

bucks = ('loc', 'mov', 'ads')
def search(*args):
    assert len(args) > 0, AssertionError('please specify at least one argument')
    buck = args[0] if args[0] in bucks else 'loc'
    term = ' '.join(map(lambda w: unicode(w, 'utf-8'), args[1:]))
    return riak.search(buck, u'loc:' + term)

def export(var):
    if isinstance(var, basestring):
        return pprint(var)
    if isinstance(var, dict):
        for k, v in var.iteritems():
            print k, ':',
            export(v)
        return
    if isinstance(var, (list, tuple, set)):
        for v in var:
            export(v)
        return

riak = RiakClient(**config.getRiak())
query = search(*sys.argv)

count = 0
for record in query.run():
    res = record.get().get_data()
    export(res)
    count += 1
    if count > 10:
        break
