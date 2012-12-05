import os, sys

from rabs import rabs
import threading

class sftp(rabs):
    
    def __init__(self, config, **callback):
        rabs.__init__(self, config)
        self.sp = self.rp.open_sftp()
        for callbackfuncname in ('callbackput', 'callbackget', 'callback'):
            if not callback.get(callbackfuncname) is None:
                self.__setattr__(callbackfuncname, callback[callbackfuncname])
        else: pass
        self.sp.chdir(self.config['path']['web'])
    
    def put(self, lfp, rfp=None, callback=None):
        self.sp.put(lfp, rfp, callback or self.callbackput or self.callback)
    
    def get(self, rfp, lfp=None, callback=None):
        self.sp.get(rfp, lfp, callback or self.callbackget or self.callback)
    
    def __getattr__(self, name):
        try:
            return self.__getattribute__(name) or self.sp.__getattribute__(name)
        except:     # abnormal
            raise Exception('%s Error: attribute %s not exists... exit!' % (self.config['sid'], name))

    def callback(self, *args):
        pass
    
    def callbackput(self, *args):
        pass
    
    def callbackget(self, *args):
        pass
