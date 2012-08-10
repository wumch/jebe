#coding:utf-8

import logging
from utils.datetimes import now

# log level settings
RiakWriteError  = 'RiakWriteError'
RiakReadError   = 'RiakReadError'

class LogHandler(logging.FileHandler):

    def format(self, record):
        return now() + ' %(levelname)s %(filename)s:%(lineno)d %(funcName)s%(args)s %(msg)s' % record.__dict__

def mklogger(logfile):
    logger = logging.getLogger()
    logger.setLevel(logging.NOTSET)
    logger.removeHandler(logger.handlers)
    logger.addHandler(LogHandler(filename=logfile))
    return logger
