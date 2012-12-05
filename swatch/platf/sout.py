#coding:utf-8
import codecs

from rabs import rabs
from app.appenv import *

class sout(rabs):
    
    def fout(self, data):
        fp = codecs.open(self.config['output']['path'], 'w', setting['OUTPUT_CODING'])
        fp.write(data)
        fp.close()
    
    def gout(self, data):
        pass
    
    def eout(self, data):
        pass
