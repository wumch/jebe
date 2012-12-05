#coding:utf-8

import os, sys
sys.path.insert(0, '/data/fsuggest/jebe/swatch/app')
from appenv import path
from servers import servers

__config = {
    'sid' : '',
    'coding' : { 'output' : 'utf-8' },
    'output' : { 'dir' : path['OUTPUT'], 'filename' : '', 'path' : '', 'suffix' : '.txt' },
    'rmysql' : { 'host' : '', 'port' : 3306, 'user' : '', 'password' : '', 'database' : '', 'charset' : 'utf8', },
    'mysql' : { 'host' : 'localhost', 'port' : 3306, 'user' : 'root', 'password' : '', 'database' : '', 'charset' : 'utf8', },
    'ssh' : { 'host' : '', 'port' : 22, 'user' : 'root', 'password' : '', 'pkey' : None, },        # os.path.dirname(os.path.realpath(__file__)) + '\\' + 'pkey' + '\\' + 'id_dsa_1024_wumch'
    'path' : { 'web' : '/www/', 'tracelog' : '' },
    'telnet' : { 'host':'', 'port':23, 'user':'anonymous', 'password':'non-password', },
}

def __initializition(sid):
    __host = servers[sid]
    
    __config['sid'] = sid
    __config['rmysql']['host'] = \
    __config['ssh']['host'] = \
    __config['telnet']['host'] = \
    __host
    
    __config['output']['filename'] = sid + __config['output']['suffix']
    __config['output']['path'] = os.path.join(__config['output']['dir'], __config['output']['filename'])
    __config['path']['tracelog'] = __config['path']['web'] + 'log/operate/'

def __servercombine(sid, hsid):
    __config['rmysql']['host'] = __import__(hsid).config['rmysql']['host']
    __config['path']['web'] = __config['path']['web'].replace(r'/www/', r'/mm-' + sid + r'/')

def __servercombinedout(sid):
    __config['path']['web'] = __config['path']['web'].replace(r'/web/', r'/web-' + sid + r'/')

def __datacombine(sid, hsid):
    __config['mysql']['database'] = \
    __config['rmysql']['database'] = \
    sid
