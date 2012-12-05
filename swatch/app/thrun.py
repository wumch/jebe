#coding:utf-8
import os, sys
import codecs

from thrunabs import thrunabs

from excelMysql import excelMysql

import param

# mysql -e 'sql'
class sqlrun(thrunabs):
    
    def _run(self, instru):
        from plist import plist
        self.vars['ids'] = plist.get(self.config['sid'])
        return self.shesql.exc(instru % self.vars)
    
# 执行单条 shell
class sherun(thrunabs):
    
    def _run(self, instru):
        return self.rexc.exc(instru % self.vars)

# trace_log 匹配
class geprun(thrunabs):
    
    def _run(self, instru):
        return self.rexc.exc(self.__decorate(instru))
    
    def __decorate(self, gep):
        return 'cd ' + self.config['path']['tracelog'] + ' && ' + gep

# odbc 远程 select
class rsqlrun(thrunabs):
    
    def _run(self, instru):
        return self.__formstr(self.rsql.query(instru))
    
    def __formstr(self, sqlret):
        strdata = ''
        for row in sqlret:
            for fieldval in row:
                strdata += unicode(fieldval) + "\t"
            strdata = strdata.rstrip() + os.linesep
        return strdata

# sftp 继承自 paramiko.SFTPClient
class sftprun(thrunabs):
    
    def _run(self, instru):
        return self.__ftran(instru)
    
    def __ftran(self, fpath):
        allsuccess = True
        from param import param
        for funcname, fpathargs in param['common'].items():
            func = getattr(self.sftp, funcname, None)
            if func is not None:
                for args in fpathargs:
                    if len(args) > 0:
                        for arg in args:
                            arg = arg % self.config['path']
                        if not apply(func, args):
                            allsuccess = False
                            self.asside.errorlog('%s Faild on: %s %s' % (self.config['sid'], funcname, fpathargs))
        return allsuccess
    
    def _argavail(self, instru):
        return False

class sqlist(thrunabs):
    
    def _run(self, instru):
        from plist import plist
        sql_list = plist.get(self.config['sid'])
        runed = ''
        if sql_list is None or len(sql_list) == 0:
            print self.config['sid'], 'no sql to run...'
            return
        for Player_ID in sql_list:
            sql = self.__decorate(Player_ID)
            res = self.shesql.exc(sql)
            runed += sql + ';' + os.linesep
        return runed
    
    def __decorate(Player_ID):
        return 'select Player_ID,max(Daughter_Reincarnation_Count) from t_daughter_backup where Player_ID=%d group by Player_ID order by Player_ID asc' % int(Player_ID)

class sqlParam(thrunabs):
    
    def _run(self, instru):
        paramList  = getattr(__import__('param.' + self.config['sid']), self.config['sid']).param
        if paramList is None or len(paramList) == 0:
            print self.config['sid'], 'Nothing to do...'
            return 'Nothing'
        return self.shesql.exc(self.__decorate(paramList))
    
    def __decorate(self, paramList):
        return 'select player_name,c.* from t_consume c left join t_player p using(player_id) where (`date` between "2010-03-22" and "2010-03-29") and player_name in ("%s") order by player_id asc,goods_id asc' % '","'.join(map(str, paramList))

class stemp(thrunabs):
    def _run(self, instru):
        import time
        days = ['2009-09-28', '2009-09-29', '2009-09-30']
        days += ['2009-10-0'+str(i) for i in xrange(1,10)]
        days += ['2009-10-'+str(i) for i in xrange(10,19)]
        sid = self.config['sid'][1:]
        sqls = ''
        for j,date in enumerate(days):
            if  j >= len(days)-1:
                print j, date
                break
            datenext = days[j+1]
            web = self.config['path']['web']
            sql = '''select concat("4|%(sid)s|",c.player_id,"|",player_name,"|",cost,"|",Goods_id,"|",`date`) from t_consume c left join t_player p on c.player_id=p.player_id where `date` between "%(date)s" and "%(datenext)s" order by `date` asc into outfile "%(web)slog/applog/consume/consume_%(date)s.log";'''
            sql = sql % locals()
            self.shesql.exc(sql)
            sqls += sql + os.linesep
        return sqls

class kill(thrunabs):
    def _run(self, procsign):
        self.rexc.exc('''kill `ps aux|grep %s|grep -v grep|awk 'BEGIN{ORS=" "}{print $2;}'`''' % procsign)

class gameRestart(thrunabs):
    def _run(self, useless):
        return self.telnet.exc('restart')
        
        