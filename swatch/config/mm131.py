#coding:utf-8
import os, sys
from copy import deepcopy

from appenv import path

__config = {
    'sid' : '',
    'coding' : { 'output' : 'utf-8' },
    'output' : { 'dir' : path['OUTPUT'], 'filename' : '', 'path' : '', 'suffix' : '.txt' },
    'rmysql' : { 'host' : 'test.mm.kunlun.com', 'port' : 3306, 'user' : 'watch', 'password' : 'watcher54612', 'database' : 'mgc', 'charset' : 'utf8', },
    'mysql' : { 'host' : 'localhost', 'port' : 3306, 'user' : 'root', 'password' : 'IGetTheWorld54612', 'database' : 'mgc_1_3_1', 'charset' : 'utf8', 'datapath':'/data/etc/mysql5/data', },
    'ssh' : { 'host' : 'test.mm.kunlun.com', 'port' : 22, 'user' : 'root', 'password' : 'qe2ao4', 'pkey' : None, },        # os.path.dirname(os.path.realpath(__file__)) + '\\' + 'pkey' + '\\' + 'id_dsa_1024_wumch'
    'path' : { 'web' : '/data/www/haifeng.li/mgc_1.3.1/', 'tracelog' : '' }
}

def __initializition(sid):
    __host = 'test.mm.kunlun.com'
    
    __config['sid'] = sid
    __config['rmysql']['host'] = \
    __config['ssh']['host'] = \
    __host
    
    __config['output']['filename'] = sid + __config['output']['suffix']
    __config['output']['path'] = os.path.join(__config['output']['dir'], __config['output']['filename'])
    __config['path']['tracelog'] = __config['path']['web'] + 'log/operate/'

__initializition('mm131')

config = deepcopy(__config)


if __name__ == '__main__':
    print(config)
