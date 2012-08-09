#coding:utf-8

import logging
from utils.datetimes import now

# log level settings
RiakWriteError  = 'RiakWriteError'
RiakReadError   = 'RiakReadError'

class LogHandler(logging.FileHandler):

    def format(self, record):
        return now() + ' ' + super(LogHandler, self).format(record)

def mklogger(logfile):
    logger = logging.getLogger()
    logger.setLevel(logging.NOTSET)
    logger.removeHandler(logger.handlers)
    logger.addHandler(LogHandler(filename=logfile))
    return logger
