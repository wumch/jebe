#coding:utf-8

import datetime
import time

from config.settings import DATE_FORMAT as DF

weekdays = ('星期一', '星期二', '星期三', '星期四',
            '星期五', '星期六', '星期日', )

def today():
    return datetime.date.today().strftime(DF)

def fordays(delta=1):
    """date of `delta` days later(forward)"""
    t = time.time() + delta * 86400
    return datetime.date.fromtimestamp(t).strftime(DF)

def last28days():
    return fordays(delta=-28)

def lastday():
    return fordays(-1)

def formonths(delta=1):
    """date of `delta` months later(forward)"""
    c = datetime.datetime.today()
    m = c.month + delta
    month = m % 12
    year = c.year + int(m / 12)
    return c.replace(year=year, month=month).strftime(DF)

def lastmonth():
    return formonths(delta=-1)

def strtotime(s, format):
    return int(time.mktime(time.strptime(s, format if format else '%Y-%m-%d')))

def getweekday(d, format=None):
    return weekdays[time.strptime(d, format if format else '%Y-%m-%d').tm_wday]

def listdate(begin='2011-01-01', end=lastday(),
        format='%Y-%m-%d', skip_lastday=False):
    start = strtotime(begin, format=format)
    finish = strtotime(end, format=format) / 86400 * 86400 \
            + (0 if skip_lastday else 86400)
    return (str(datetime.date.fromtimestamp(t)) \
            for t in xrange(start, finish, 86400))
