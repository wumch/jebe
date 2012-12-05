#coding:utf-8

import os, sys
import codecs

if os.name == 'nt':
    __errorlogpath = os.path.join(ur'C:\Documents and Settings\Administrator\桌面\serror.log')
    __applogpath = os.path.join(ur'C:\log\sapp.log')
    __outputpath = os.path.join(ur'C:\sdata\plain')
    __eoutputpath = os.path.join(ur'C:\sdata\excel')
    __workbench = os.path.join(ur'C:\Documents and Settings\Administrator\桌面')
    __totalres = os.path.join(ur'C:\sdata\所有服务器.txt')
    __tablefolder = os.path.join(ur'D:', ur'数据表')
else:
    __errorlogpath = os.path.join(os.sep, 'var', 'log', 'serror.log')
    __applogpath = os.path.join(os.sep, 'var', 'log', 'sapp.log')
    __outputpath = os.path.join(os.sep, 'tmp', 'plain')
    __eoutputpath = os.path.join(os.sep, 'tmp', 'excel')
    __workbench = os.path.expanduser('~')
    __totalres = os.path.join(os.sep, 'tmp', ur'所有服务器.txt')
    __tablefolder = os.path.join(os.sep, 'tmp', ur'数据表')

for __apath in (__outputpath, __eoutputpath, __workbench, ):
    if not os.path.isdir(__apath):
        try:
            os.mkdir(__apath)
        except Exception, e:
            raise IOError('Error: directory ' + __apath + ' not exists... exit!')
else: pass

for __afile in (__errorlogpath, __totalres, __applogpath, ):
    try:
        codecs.open(__afile, 'w').close()
    except Exception, e:
        raise IOError('Error: file ' + __afile + ' access denied... exit!')
else: pass

# path or env
path = {}
path['ROOT'] = os.path.dirname(os.path.dirname(os.path.realpath(__file__))) + os.sep
path['CONFIG'] = path['ROOT'] + 'config' + os.sep
path['PARAM'] = path['ROOT'] + 'param' + os.sep
path['PLATF'] = path['ROOT'] + 'platf' + os.sep
path['DB_MODULE'] = path['ROOT'] + 'db' + os.sep
path['OUTPUT'] = __outputpath
path['ERROR_LOG'] = __errorlogpath
path['APP_LOG'] = __applogpath
path['WORKBENCH'] = __workbench
path['TOTAL_RES'] = __totalres
path['TABLE_FOLDER'] = __tablefolder

sys.path.append(path['ROOT'])
sys.path.append(path['CONFIG'])
sys.path.append(path['PARAM'])
sys.path.append(path['PLATF'])
sys.path.append(path['DB_MODULE'])

# setting of env
setting = {}
setting['OUTPUT_CODING'] = 'utf-8'
setting['SERVER_TIMEOUT'] = 10
setting['LOG_ERROR'] = True
