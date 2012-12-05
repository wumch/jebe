#!/usr/bin/env python
#coding:utf-8

import os, sys
import codecs

from appabs import appabs
from appenv import *

import thrun

class appsample(appabs):
    
    def _spick(self):
        return ['h4', 'h5']
    
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

account = { 'h5' : [], 'h2' : [] }
account['h5'].append('mmkuafuzhan121')
account['h5'].append('mmkuafuzhan122')
account['h5'].append('mmkuafuzhan123')
account['h5'].append('mmkuafuzhan124')
account['h5'].append('mmkuafuzhan125')
account['h5'].append('mmkuafuzhan126')
account['h5'].append('mmkuafuzhan127')
account['h5'].append('mmkuafuzhan128')
account['h5'].append('mmkuafuzhan129')
account['h5'].append('mmkuafuzhan130')
account['h5'].append('mmkuafuzhan131')
account['h5'].append('mmkuafuzhan132')
account['h5'].append('mmkuafuzhan133')
account['h5'].append('mmkuafuzhan134')
account['h5'].append('mmkuafuzhan135')
account['h5'].append('mmkuafuzhan136')
account['h5'].append('mmkuafuzhan137')
account['h5'].append('mmkuafuzhan138')
account['h5'].append('mmkuafuzhan139')
account['h5'].append('mmkuafuzhan140')
account['h2'].append('mmkuafuzhan031')
account['h2'].append('mmkuafuzhan032')
account['h2'].append('mmkuafuzhan033')
account['h2'].append('mmkuafuzhan034')
account['h2'].append('mmkuafuzhan035')
account['h2'].append('mmkuafuzhan036')
account['h2'].append('mmkuafuzhan037')
account['h2'].append('mmkuafuzhan038')
account['h2'].append('mmkuafuzhan039')
account['h2'].append('mmkuafuzhan040')
account['h2'].append('mmkuafuzhan041')
account['h2'].append('mmkuafuzhan042')
account['h2'].append('mmkuafuzhan043')
account['h2'].append('mmkuafuzhan044')
account['h2'].append('mmkuafuzhan045')
account['h2'].append('mmkuafuzhan046')
account['h2'].append('mmkuafuzhan047')
account['h2'].append('mmkuafuzhan048')
account['h2'].append('mmkuafuzhan049')
account['h2'].append('mmkuafuzhan050')
account['h2'].append('mmkuafuzhan051')
account['h2'].append('mmkuafuzhan052')
account['h2'].append('mmkuafuzhan053')
account['h2'].append('mmkuafuzhan054')
account['h2'].append('mmkuafuzhan055')
account['h2'].append('mmkuafuzhan056')
account['h2'].append('mmkuafuzhan057')
account['h2'].append('mmkuafuzhan058')
account['h2'].append('mmkuafuzhan059')
account['h2'].append('mmkuafuzhan060')

if __name__ == '__main__':
    if setting['LOG_ERROR'] is True:
        sys.stderr = codecs.open(path['ERROR_LOG'], 'a')
    from thinstr import thinstr
    for modulename, args in thinstr.items():
        if args and (not isinstance(args, str) or args.strip() != ''):
            if modulename == 'sherun':
                appsample(getattr(thrun, modulename)).run()
    else: pass
    