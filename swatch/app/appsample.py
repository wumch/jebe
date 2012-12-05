#!/usr/bin/env python
#coding:utf-8

import os, sys
import codecs

from appabs import appabs
from appenv import *

import thrun

class appsample(appabs):
    
    def _spick(self):
        return ['mm131']
        #return ['h1']
        #sids = ['s' + str(i) for i in xrange(4,19)]
        #sids.append('h1')
        #return sids
        #return ['h5', 's19']
        sids = ['h' + str(i) for i in xrange(1,6)]
        sids.append('s19')
        return sids
    
    def _prepare(self):
        print('Go Go Go ...')
    
    def _end(self):
        fp = codecs.open(path['TOTAL_RES'], 'a', setting['OUTPUT_CODING'])
        self.sids.sort(cmp=lambda a,b: cmp(int(a[1:]), int(b[1:])))
        for sid in self.sids:
            fp.write(sid + ':' + os.linesep)
            fp.write(self.res[sid])
            fp.write(os.linesep + os.linesep)
        else: fp.close()
        #os.system(r'"C:\Program Files\WinRAR\Rar.exe" a sdata.zip "%s\*"' % path['OUTPUT'])

if __name__ == '__main__':
    if setting['LOG_ERROR'] is True:
        sys.stderr = codecs.open(path['ERROR_LOG'], 'a')
    from thinstr import thinstr
    for modulename, args in thinstr.items():
        if args and (not isinstance(args, str) or args.strip() != ''):
            if modulename == 'excelMysql':
                appsample(getattr(thrun, modulename)).run()
    else: pass
    