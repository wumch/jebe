#coding:utf-8

import os, sys
sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__))), 'service')
from riak import RiakClient
from config import config

riak = RiakClient(**config.getRiak())
loc = riak.bucket('loc')
query = riak.search('loc', 'loc:[http]')

count = 0
for record in query.run():
    res = record.get().get_data()
    print res
    count += 1
    if count > 10:
        break
