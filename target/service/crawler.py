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
    ('Content-Type', 'application/x-javascript; charset=utf-8'),
    ('Pragma', 'no-cache'),
]
status_ok = '200 OK'
status_error = '500 Internal Error'

def application(environ, start_response):
    global headers, status_ok, status_error
    res = WsgiCrawler(environ=environ, start_response=start_response).handle()
    if res or isinstance(res, basestring):
        start_response(status_ok, headers)
    else:
        start_response(status_error, headers)
    return res if isinstance(res, basestring) else ''
