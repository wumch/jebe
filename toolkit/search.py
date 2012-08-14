#!/usr/bin/env python
#coding:utf-8

import os, sys
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__)), 'target', 'service'))
from riak import RiakClient
from config import config
from pprint import pprint

bucks = {u'loc':u'words', u'mov':u'ref', u'ads':u'words'}
default_buck = u'loc'
def search(*args):
    assert len(args) > 0, AssertionError('kid, please specify at least one argument')
    buck = field = None
    if len(args) > 1:
        if '.' in args[0]:
            buck, field = map(lambda w: unicode(w, 'utf-8'), args[0].split('.'))
        else:
            buck = unicode(args[0], 'utf-8')
    buck_specified = buck in bucks
    if buck_specified:
        if field is None:
            field = bucks[buck]
    else:
        buck, field = default_buck, bucks[default_buck]
    term = field + u':' + u' '.join(map(lambda w: unicode(w, 'utf-8'), args[buck_specified:]))
    print u'search ' + term + u' on ' + buck
    return riak.search(buck, term)

def export(var):
    if isinstance(var, basestring):
        print(var)
        return
    if isinstance(var, dict):
        for k, v in var.iteritems():
            print k, ':',
            export(v)
        return
    if isinstance(var, (list, tuple, set)):
        for v in var:
            export(v)
        return
    print var

riak = RiakClient(**config.getRiak())
query = search(*sys.argv[1:])

count = 0
for record in query.run():
    res = record.get().get_data()
    export(res)
    count += 1
    if count > 10:
        break
