#coding:utf-8

import pyodbc

class sexcel:
    
    def __init__(self, filename, sheetname=None):
        self.filename = filename
        self.sheetname = sheetname
        self.cur = pyodbc.connect(u'Driver={Microsoft Excel Driver (*.xls)};FIRSTROWHASNAMES=1;READONLY=FALSE;CREATE_DB="%s;DBQ=%s' % (self.filename, self.filename), autocommit=True).cursor()
        #self.cur.execute(u"CREATE TABLE `%s`(`玩家id` NUMBER,`账号` TEXT,`女儿名字` TEXT)" % (self.sheetname, ))
    
    def insert(self, param, tablename = None):
        synbool = [342537500, 'wumengchun', u'至尊小果冻']
        self.cur.execute(u'insert into `' + (None or self.sheetname) + '` values('+str(synbool[0])+', "'+synbool[1]+'", "'+synbool[2]+'")')

ep = sexcel(ur'C:\sdata\excel.total.xls', 's1aaaaa')
ep.insert({'玩家id':342537500, '账号':'wumengchun', '女儿名字':u'至尊小果冻'})
