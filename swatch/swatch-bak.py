#coding:utf-8

#
# nothing of the author's business.
#

import os, sys
import curses, traceback
import locale
import time

locale.setlocale(locale.LC_ALL, '')
encoding = locale.getpreferredencoding()
statwin_height = 8

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
        self.win.insstr(str + os.linesep, attr)
        if not nonl:
            self.win.insertln()
        self.win.box(curses.ACS_VLINE, curses.ACS_HLINE)
        self.win.refresh()

    def settitle(self, title):
        decorater = '- ' * ((self.width - len(title)) >> 2)
        title = decorater + title + ' ' + decorater
        self.win.move(1, 2)
        self.appendline(title, attr=curses.A_BOLD, nonl=True)
        self.win.move(2, 2)

class StatWin(Window):

    def __init__(self, parent):
        super(StatWin, self).__init__(parent=parent)

    def _config(self):
        self.height = statwin_height
        self.words_per_page = 323

    def _prepare(self):
        self.settitle('statistics and prediction')

    def grid(self):
        pass

class CrawlWin(Window):

    def __init__(self, parent):
        super(CrawlWin, self).__init__(parent=parent)

    def _config(self):
        self.height = curses.LINES - statwin_height
        self.y = statwin_height
        self.prefix = 'crawling:'

    def _prepare(self):
        self.settitle('crawling')

    def show(self):
        self.appendline("fsadfsadfasdf")
        for line in sys.stdin:
            self.appendline(self.getUrl(line))

    def getUrl(self, line):
        return line[27:]

def run(scr):
    statwin = StatWin(scr)
    crawlwin = CrawlWin(scr)
    crawlwin.show()
    lineno = 1
    while True:
        statwin.appendline("%d ffd" % lineno, attr=curses.A_COLOR)
        lineno += 1
        statwin.refresh()
        time.sleep(1)

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
        if curses.has_colors() and curses.can_change_color():
            curses.start_color()
            pair_number = curses.pair_number(curses.color_pair(curses.COLOR_BLUE))
            curses.init_pair(pair_number, curses.COLOR_WHITE, curses.COLOR_BLUE)
            curses.color_content(curses.COLOR_RED)
            scr.bkgdset(curses.COLOR_BLUE)
        curses.def_prog_mode()
        curses.noecho()
        curses.cbreak()
        scr.keypad(1)

        run(scr=scr)
        clean(scr=scr)
    except KeyboardInterrupt:
        clean(scr=scr)
    except:
        clean(scr=scr)
        traceback.print_exc()

if __name__ == '__main__':
    main()
