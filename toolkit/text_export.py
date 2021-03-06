#!/usr/bin/env python
#coding:utf-8

import os, sys
src_path = os.path.join(os.path.dirname(os.path.dirname(os.path.realpath(__file__))), 'target', 'service')
if src_path not in sys.path:
    sys.path.append(src_path)
import codecs
import time
import pymongo
from config import config

class TextExporter(object):

    sleep_interval = 10000
    sleep_millsecs = 100

    def __init__(self):
        server = config.getMongoDB('text')
        self.connection = pymongo.Connection(**server['param'])
        self.collection = self.connection[server['db']][server['collection']]

    def export(self, output_file, max_scan):
        handler = self.handle(output_file=output_file)
        counter = 0
        for entry in self.collection.find(None, fields=['text'], limit=max_scan, sort=[("$natural", 1)]):
            handler(entry)
            counter += 1
            if counter > self.sleep_interval:
                counter = 0
                time.sleep(self.sleep_millsecs / 1000.0)

    def handle(self, output_file):
        fp = codecs.open(output_file, mode='a', encoding='utf-8', buffering=100 << 10)
        def _handle(entry):
            fp.write(entry['text'])
            fp.write(os.linesep)
        return _handle

    def __del__(self):
        self.connection.close()

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print """usage: %s <output-file> [max-scan=1,000,000] ...""" % sys.argv[0]
        sys.exit(1)
    TextExporter().export(sys.argv[1], max_scan=int(sys.argv[2]) if len(sys.argv) > 2 else 1000000)
