import os, sys

import paramiko

from app.appenv import setting
import threading

class rabs:
    
    def __init__(self, config = {}):
        self.config = config
        self.rp = self.__getrp()
    
    def __getrp(self):
        ssh = paramiko.SSHClient()
        ssh.set_missing_host_key_policy(paramiko.AutoAddPolicy())
        # will create threads for every connect
        ssh.connect(self.config['ssh']['host'], port=self.config['ssh']['port'], username=self.config['ssh']['user'], password=self.config['ssh']['password'], key_filename=self.config['ssh']['pkey'], timeout=setting['SERVER_TIMEOUT'])
        return ssh
    
    def exc(self, sen):
        pass
    
    def _shell(self, cmd):
        sin, sout, serr = self.rp.exec_command(cmd, 32*1024*1024)
        errmsg = serr.read().strip().replace("\n", os.linesep)
        if errmsg:
            print self.config['sid'] + ' shell error :' + os.linesep + errmsg
        return sout.read().replace("\n", os.linesep)
