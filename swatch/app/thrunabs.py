import os, sys
import time
import threading
import codecs

from platf import *
from appenv import path
from model import mysql

from thinstr import thinstr

glock = threading.RLock()

class thrunabs(threading.Thread):
    
    def __init__(self, config):
        glock.acquire()
        self.config = config
        threading.Thread.__init__(self, name=self.config['sid'])
        self.asside = asside.asside.getinstance(asside.asside)
        self.res = u''
        self.vars = self.config
        self.vars.update(self.config['path'])
        glock.release()
    
    def _done(self):
        self.asside.measure.append(self.config['sid'])
        print(self.config['sid'] + ' done ...')
    
    def __done(self):
        glock.acquire()
        self._done()
        glock.release()
    
    def _run(self, instru):
        pass
    
    def run(self):
        self._runproxy()
        self._output()
        self.__done()
        return self.res
    
    def _runproxy(self):
        instru = thinstr.get(self.__class__.__name__)
        if self._argavail(instru):
            self.res = self._run(self.__decorate(instru))
        if isinstance(self.res, str):
            self.res = unicode(self.res, self.config['coding']['output'])
    
    def _argavail(self, instru):
        return not not (instru and instru.strip())
    
    def _output(self):
        if self.res is not None: self.out.fout(self.res)
    
    def __decorate(self, instru):
        return instru.strip()
    
    def __getattr__(self, name):
        try:
            return self.__getattribute__(name)
        except:
            attrobj = None
            if name == 'rexc':
                attrobj = rexc.rexc(self.config)
            elif name == 'shesql':
                attrobj = shesql.shesql(self.config)
            elif name == 'rsql':
                attrobj = mysql.mysql(self.config['sid'])
            elif name == 'out':
                attrobj = sout.sout(self.config)
            elif name == 'sftp':
                attrobj = sftp.sftp(self.config)
            elif name == 'telnet':
                attrobj = telnet.TelnetClient(self.config)
            else:
                attrobj = self.__dynamicAttrGen(name)
            
            if not attrobj is None:
                self.__setattr__(name, attrobj)
            else:
                raise Exception('%s has no attribute %s' % (self, name))
            return attrobj
        return None
    
    def __dynamicAttrGen(self, name):
        tmpfileName = 'wumch_py_' + time.strftime('%Y-%m-%d_%H-%M-%S') + '.tmp'
        if name == '_remoteTmpfile':
            return '/tmp/' + tmpfileName 
        if name == '_localTmpfile':
            def writeLocalTmpfile(content=None):
                tmpfile = os.path.join(path['WORKBENCH'], tmpfileName)
                fp = codecs.open(tmpfile, 'w', self.config['mysql']['charset'])
                if content is not None:
                    fp.write(content)
                fp.close()
                return tmpfile
            return writeLocalTmpfile
        