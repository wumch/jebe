#!/usr/bin/env python
#coding:utf-8

import os, sys
import scipy

class WordsMerge(object):

    def __init__(self, outfile, dictfile, being_merged_file, delimiter="\t", headers=0):
        self.outfile = outfile
        self.dictfile = dictfile
        self.bmfile = being_merged_file
        self.delimiter = delimiter
        self.headers = headers

    def _prepareDictionary(self):
        self.dic = self._parseFile(self.dictfile, headers=self.headers)
        self.bmd = self._parseFile(self.bmfile, headers=0)

    def process(self):
        self._analysis()
        self._merge()
        self._filter()
        self._dump()

    def _analysis(self):
        self._prepareDictionary()
        arr_d = scipy.array(self.dic.values(), scipy.int32)
        arr_b = scipy.array(self.bmd.values(), scipy.int32)
        median_d = scipy.median(arr_d)
        median_b = scipy.median(arr_b)
        mean_d = arr_d.mean()
        mean_b = arr_b.mean()
        self.rate = scipy.sqrt(median_b) * median_d / median_b

        tpl = "%s\t%s\t%s"
        print tpl % (r"a\b", "median", "mean")
        tpl = "%s\t%d\t%d"
        print tpl % ("dict", median_d, mean_d)
        print tpl % ("bmf", median_b, mean_b)

    def _merge(self):
        for word_b, stime in self.bmd.iteritems():
            atime_b = self._genAtime(stime)
            if word_b not in self.dic:
                self.dic[word_b] = atime_b

    def _genAtime(self, stime):
        return int(self.rate * scipy.sqrt(stime))

    def _parseFile(self, filename, headers=0):
        ifp = open(filename, 'r')
        res = {}
        skiped = 0
        for line in ifp:
            if skiped < headers:
                skiped += 1
                continue
            key, atime = self._parseLine(line=line)
            if key not in res:
                res[key] = atime
            else:
                res[key] += atime
        ifp.close()
        return res

    def _filter(self):
        skipKeys = filter(lambda word: not len(word) > (2 if word.isalnum() else 3), self.dic)
        for k in skipKeys:
            self.dic.pop(k)

    def _dump(self):
        ofp = open(self.outfile, 'w')
        totalWords = len(self.dic)
        totalAtime = sum(self.dic.values())
        ofp.write(("%d\t%d" + os.linesep) % (totalWords, totalAtime))
        for word, atime in self.dic.iteritems():
            ofp.write(word + '\t' + str(atime) + os.linesep)
        ofp.close()

    def _parseLine(self, line):
        info = line.split(self.delimiter)
        return info[0].strip(), int(info[1].replace(',', '') if ',' in info[1] else info[1])

if __name__ == '__main__':
    if len(sys.argv) < 4:
        print "usage: %s <output-file> <dictionary-file> <being-merged-file> [being-merged-file...]" % sys.argv[0]
        sys.exit(1)
    output_file, dictfile, being_merged_file = sys.argv[1], sys.argv[2], sys.argv[3]
    WordsMerge(
        outfile=output_file,
        dictfile=dictfile,
        being_merged_file=being_merged_file,
        delimiter='\t',
        headers=0,
    ).process()
