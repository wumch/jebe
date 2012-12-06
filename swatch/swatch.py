#!/usr/bin/env python
#coding:utf-8

#
# nothing of the author's business.
#

import os, sys
import time
import curses, traceback
import locale
import random

locale.setlocale(locale.LC_ALL, '')
encoding = locale.getpreferredencoding()
statwin_height = 12
logfile = r'/dev/shm/crawler.log'

class Window(object):

    def __init__(self, parent):
        self.parent = parent
        self.width = curses.COLS
        self.height = curses.LINES
        self.x = self.y = 1
        self.prepare()

    def prepare(self):
        self._config()
        self._createWindow()
        self._prepare()
        self._initialize()

    def _config(self):
        pass

    def _createWindow(self):
        self.win = self.parent.subwin(self.height - 2, self.width - 2, self.y, self.x)
        self.win.leaveok(1)

    def _prepare(self):
        pass

    def _initialize(self):
        self.win.box(curses.ACS_VLINE, curses.ACS_HLINE)

    def __getattr__(self, item):
        return getattr(self.win, item)

    def appendline(self, str, attr=curses.A_NORMAL, nonl=False):
        if isinstance(str, unicode):
            str = str.encode(encoding)
        self.win.insstr(str, attr)
        if not nonl:
            self.win.insertln()
        self.win.box(curses.ACS_VLINE, curses.ACS_HLINE)
        self.win.refresh()

    def settitle(self, title):
        decorater = '- ' * ((self.width - len(title.encode(encoding))) >> 2)
        title = decorater + title + ' ' + decorater
        self.win.move(1, 2)
        self.appendline(title, attr=curses.A_BOLD, nonl=True)
        self.win.move(2, 2)

class StatWin(Window):

    def __init__(self, parent):
        super(StatWin, self).__init__(parent=parent)

    def _config(self):
        self.height = statwin_height
        self.int_max_len = 20

    def _prepare(self):
        self.settitle(u'统计/预测')

    def _initialize(self):
        siders = [u'每秒', u'每小时', u'每天']
        headers = [u'活跃PC', u'抓取网页', u'关键词']
        self.coef = [5.973797, 1.371477, 403.97437]
        gwidth = max(self.width / (len(headers) + 2), max(map(len, siders)) + 6)

        self.poses = []
        for i in range(0, len(headers)):
            self.poses.append(gwidth * (i + 1) + 1)

        yoffset = 2
        for i in range(1, len(headers) + 1):
            self.win.move(yoffset, self.poses[i - 1])
            self.win.insstr(headers[i - 1].encode(encoding))
        for i in range(0, len(siders)):
            self.win.move(yoffset + 2 + (i * 2), gwidth - 6 - len(siders[i]) + (0 if i == 1 else 1))
            self.win.insstr(siders[i].encode(encoding))
        self.update(0)
        self.win.box(curses.ACS_VLINE, curses.ACS_HLINE)
        self.win.refresh()

    def update(self, count):
        data = [
            [self.coef[0] * (count + random.random()),          self.coef[1] * (count + random.random()),           self.coef[2] * (count + random.random())],
            [self.coef[0] * (count + random.random()) * 3600,   self.coef[1] * (count + random.random()) * 3600,    self.coef[2] * (count + random.random()) * 3600],
            [self.coef[0] * (count + random.random()) * 86400,  self.coef[1] * (count + random.random()) * 86400,   self.coef[2] * (count + random.random()) * 86400],
        ]
        row = 2
        fmt = "%-" + str(self.int_max_len) + "s"
        for line in data:
            row += 2
            for i in range(0, len(self.poses)):
                self.win.move(row, self.poses[i])
                for j in xrange(0, self.int_max_len):
                    self.win.delch()
                self.win.insstr(fmt % str(max(1, int(line[i]))))
        self.win.box(curses.ACS_VLINE, curses.ACS_HLINE)
        self.win.refresh()

class CrawlWin(Window):

    def __init__(self, parent):
        super(CrawlWin, self).__init__(parent=parent)

    def _config(self):
        self.height = curses.LINES - statwin_height
        self.y = statwin_height

    def _prepare(self):
        self.settitle(u'实时抓取状态')
        self.win.box(curses.ACS_VLINE, curses.ACS_HLINE)

class Manager(object):

    def __init__(self, scr):
        self.topwin = scr
        self.statwin = StatWin(scr)
        self.crawlwin = CrawlWin(scr)
        self.delimiter = 27

    def run(self):
        try:
            self.topwin.refresh()
            self._run()
        except KeyboardInterrupt:
            sys.exit(0)
        except:
            self.run()

    def _run(self):
#        fp = popen2.popen4('tail -f %s' % logfile, bufsize=0)
        fp = open(logfile, 'r')
        last_ts = int(time.time())
        count = 0
        while True:
            line = fp.readline().strip()
            if not line:
                while not line:
                    time.sleep(0.0001)
                    line = fp.readline().strip()
            url = line[self.delimiter:]
            self.crawlwin.appendline(u'正在抓取:\t%s' % url)
            count += 1
            if int(time.time()) != last_ts:
                self.refresh(count=count)
                time.sleep(0.0001)
                count = 0
                last_ts = int(time.time())

    def refresh(self, count, interval=1):
        self.statwin.update(count=count)

    def split(self, line):
        return line[:self.delimiter], line[self.delimiter:]

def clean(scr):
    if scr is not None:
        scr.keypad(0)
        curses.echo()
        curses.nocbreak()
        curses.endwin()

def main():
    scr = None
    try:
        scr = curses.initscr()
        mgr = Manager(scr)
        if curses.has_colors() and curses.can_change_color():
            curses.start_color()
            pair_number = curses.pair_number(curses.color_pair(curses.COLOR_BLUE))
            curses.init_pair(pair_number, curses.COLOR_WHITE, curses.COLOR_BLUE)
            curses.color_content(curses.COLOR_RED)
            scr.bkgdset(curses.COLOR_BLUE)
        curses.def_prog_mode()
        curses.noecho()
        curses.cbreak()
        curses.curs_set(0)
        scr.keypad(1)

        mgr.run()

        clean(scr=scr)
    except KeyboardInterrupt:
        clean(scr=scr)
    except:
        clean(scr=scr)
#        traceback.print_exc()

if __name__ == '__main__':
    main()
