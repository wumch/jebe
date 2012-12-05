import os, sys
import re
import threading
from copy import deepcopy

from appenv import *
from config import *
from platf.asside import asside

class appabs:
    
    def __init__(self, thrun):
        self.tpool = {}
        self.thrun = thrun
        self.res = {}
        self.sids = self._spick()
        self.waiter = threading.Thread(target=self.__end)
        self._prepare()
    
    def _spick(self):
        # default: all of servers
        return filter(None, map(self.__sidsep, os.listdir(path['CONFIG'])))
    
    def _prepare(self):
        # run before distributed-run
        pass
    
    def _end(self):
        # run on all done
        pass
    
    def __aend(self):
        for sid, thd in self.tpool.items():
            self.res[sid] = thd.res
        self.__notice()
        self._end()
    
    def __notice(self):
        sdone = self.tpool[self.sids[0]].asside.measure         # not-standard
        sdone.sort(cmp=lambda a,b:cmp(int(a[1:]), int(b[1:])))
        print('done: %s%smiss: %s' % (sdone, os.linesep, filter(lambda sid:not sdone.__contains__(sid), self.sids)))
    
    def run(self):
        # multi threads
        self.__drun()
    
    def __drun(self):
        for sid in self.sids:
            self.tpool[sid] = self.thrun(self.__getconf(sid))
        else:
            for sid, thd in self.tpool.items():
                thd.start()
            else:
                self.waiter.start()
        pass
    
    def __end(self):
        for sid, thd in self.tpool.items():
            thd.join()
        self.__aend()
    
    def __getconf(self, sid):
        if sid == 'test':
            from config import test
            return test.config
        return __import__(sid, fromlist=['config']).config
    
    def __sidsep(self, filename):
        if not self.__sidreg:
            self.__sidreg = re.compile(r'^s[\d]*$')
        sid = filename.split('.')[0]
        if self.__sidreg.match(sid) is None:
            sid = False
        return sid
