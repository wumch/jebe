#!/usr/bin/env python
#coding:utf-8

try:
    import psyco
    psyco.jit()
    from psyco.classes import *
except ImportError:
    pass

from controler.wsgi_crawler import WsgiCrawler

headers = [
#    ('Connection', 'close'),
    ("Content-Type", "application/x-javascript; charset=utf-8"),
    ("Cache-Control", "no-store, no-cache, must-revalidate, post-check=0, pre-check=0"),
#    ('Pragma', 'no-cache'),
]
status_ok = '200 OK'
status_error = '500 Internal Err'
status_wrong = '405 Invalid Request'

def application(environ, start_response):
    global headers, status_ok, status_error
    res = WsgiCrawler(environ=environ, start_response=start_response).handle()
    if isinstance(res, int):
        start_response(status_wrong, headers)
    elif res or isinstance(res, basestring):
        start_response(status_ok, headers)
    else:
        start_response(status_error, headers)
    return res if isinstance(res, basestring) else ''
