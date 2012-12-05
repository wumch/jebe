#coding:utf-8

import os, sys
import threading
import codecs
import time

sys.path.append(os.path.join(os.path.dirname(os.path.dirname(__file__))))

from app.appenv import *
glock = threading.RLock()

glock.acquire()

class asside:
    # singleton
    __instance = None
    
    def __init__(self):
        glock.acquire()
        if asside.__instance is not None:
            raise Exception('duplicate singleton:%s' % self.__class__.__name__)
        self.measure = []
        glock.release()
    
    @staticmethod
    def getinstance(cls):
        glock.acquire()
        if not isinstance(cls.__instance, cls):
            cls.__instance = cls()
        return cls.__instance
        glock.release()
    
    def log(self, utext, sid=None):
        self.__log(codecs.open(path['APP_LOG'], 'a', setting['OUTPUT_CODING']) or sys.stdout, utext, sid)
    
    def errorlog(self, utext, sid=None):
        self.__log(codecs.open(path['ERROR_LOG'], 'a', setting['OUTPUT_CODING']) or sys.stderr, utext, sid)
    
    def __log(self, fp, utext, sid=None):
        fp.write('%s%s:%s%s%s' % (sid and sid+' ' or '', time.strftime(time.strftime('%Y-%m-%d %H:%M:%S')), os.linesep, unicode(utext), os.linesep + os.linesep))
        if not (sys.stderr, sys.stdout, ).__contains__(fp):
            fp.close()
    
    # 强制同步。暂时没实际效果。
    def __getattribute__(self, name):
        glock.acquire()
        try:
            return getattr(self, name)
        except:
            raise Exception('%s has no attribute %s' % (self, name))
        finally:
            glock.release()

glock.release()
