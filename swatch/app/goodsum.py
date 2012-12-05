#!/usr/bin/env python
#coding:utf-8

import os, sys
import codecs

from appabs import appabs
from appenv import *

import thrun

class goodsum(appabs):
    
    def _spick(self):
        return ['s' + str(i) for i in xrange(1,19)]
    
    def _prepare(self):
        print('goodsum.py Go Go Go ...')
    
class rsqlrun(thrunabs):
    
    def _run(self, instru):
        __goodslists = file(r'C:\Documents and Settings\Administrator\桌面\8.25_商城出错物品Id.txt').readlines()
        goodslists = []
        for line in __goodslists:
            info = line.split("\t")
            goodslists.append({'Goods_ID' : info[0], 'Goods_Name' : info[1]})
    
    def __single(self, gid):
        return self.rsql.query('select sum(Stuff_StuffCount) from t_daughter_stuff where stuff_stuffid=' + gid)
