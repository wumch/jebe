#!/usr/bin/env python
#coding:utf-8
import os, sys
import codecs

from pyExcelerator import *

from appenv import *
from thrunabs import thrunabs

class excelMysql(thrunabs):
    
    def _run(self, excelFileName):
        if self.config['ssh']['host'] not in ('test.mm.kunlun.com', ):
            print "don't disorder on %(sid)s !!!" % self.vars
            os._exit(0)
        self.tableName = excelFileName[:excelFileName.find('.')]
        sqlContent = self.__sqlGen(excelFileName)
        tmpfile = self._localTmpfile(sqlContent)
        self.sftp.put(tmpfile, self._remoteTmpfile)
        self.__mysqlTableFileBackup(self.tableName)
        output = self.shesql.exc('source %s;' % self._remoteTmpfile) or 'nothing'
        self.rexc.exc('find %(web)scache -type f|xargs rm -f' % self.vars)
        return output
    
    def __mysqlTableFileBackup(self, tableName):
        try:
            fileShellExp = r'%s/%s/%s.*' % (self.config['mysql']['datapath'], self.config['mysql']['database'], tableName)
            backupDir = r'%s/%s/wumch/' % (self.config['mysql']['datapath'], self.config['mysql']['database'])
            shellExp = 'cp -f %s %s' % (fileShellExp, backupDir)
            self.rexc.exc(shellExp)
            print '%(sid)s backup success' % self.vars
        except:
            print '%(sid)s backup faild !!!' % self.vars
    
    def __getExcelData(self, fpath):
        return parse_xls(fpath)[0][1]
    
    def __sqlGen(self, excelFileName):
        fpath = excelFileName
        if not os.path.isfile(fpath):
            fpath = os.path.join(path['TABLE_FOLDER'], fpath)
        
        if fpath[-4:] == '.xls':
            data = self.__getExcelData(fpath)
            sqlContent = self.__sqlFormat(data)
        else:
            fp = codecs.open(fpath, 'r', 'utf-8')
            sqlContent = fp.read()
        
        return sqlContent
    
    def __sqlFormat(self, data):
        sql = 'delete from `%s`; replace into `%s` (`' % (self.tableName, self.tableName)
        fields = self.__getFields(data)
        sql += '`,`'.join(fields)
        sql += '`) values '
        
        cursor = self.metaLinenumber
        columnBound = xrange(0, len(fields))
        rows = []
        while True:
            cursor += 1
            row = []
            for coln in columnBound:
                fieldValue = data.get((cursor, coln))
                if isinstance(fieldValue, float) and int(fieldValue) == fieldValue:
                    fieldValue = int(fieldValue)
                row.append(fieldValue)
            if len(filter(None, row)) == 0:
                break
            rows.append(self.__rowFormat(row))
        sql += (',' + os.linesep).join(rows)
        sql += ';'
        return sql
    
    def __rowFormat(self, row):
        return "(%s)" % ','.join(map(lambda fieldValue: "NULL" if fieldValue is None else "'%s'" % (fieldValue,), row))
    
    def __getFields(self, data):
        self.metaLinenumber = 1
        # bugs 根据前3行值特征 猜哪行是 字段名
        import re
        fieldNameRegexp = re.compile('^\w{1,50}$')
        
        meta = []
        for i in xrange(0, 6):
            meta = []
            if not data.has_key((i,0)):
                continue
            
            MaybeMeta = True
            for j in xrange(100000):
                if not data.has_key((i,j)):
                    break       # 列必须连续,否则此列之后被忽略
                if not isinstance(data[(i,j)], basestring) or not fieldNameRegexp.match(data[(i,j)]):
                    MaybeMeta = False
                    break
                else:
                    meta.append(data[(i,j)])
            
            if MaybeMeta and len(meta) > 1:
                self.metaLinenumber = i
                break
        
        return meta
