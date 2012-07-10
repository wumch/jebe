#!/usr/bin/env python
#coding:utf-8

from __future__ import division
import os, sys
from math import log
from gb2312 import charmap
import codecs
from url_decode import urldecode

class PhraseList(object):
    def __init__(self, size):
        self.__size = size
        self.phrases = {}

    @property
    def size(self):
        return self.__size

    def addPhrase(self, phrase):
        if phrase not in self.phrases:
            self.phrases[phrase] = 0
        self.phrases[phrase] += 1

    def countPrefix(self, prefix):
        plen = len(prefix)
        assert plen < self.size
        pnum, acount = 0, 0
        for phrase in self.phrases:
            if phrase[:plen] == prefix:
                pnum += 1
                acount += self.phrases[phrase]
        return pnum, acount

    def count(self, phrase):
        return self.phrases[phrase] if phrase in self.phrases else 0

    def __repr__(self):
        return str(self.phrases) + os.linesep

class DictBuilder(object):
    def __init__(self, maxWordLen = 7):
        self.maxWordLen = maxWordLen
        self.phraseLists = [PhraseList(i + 1) for i in range(0, self.maxWordLen)]

    def scan(self, doc):
        clen = len(content)
        i, stopPoint, hasChs = 0, 0, False
        while i < clen:
            if not charmap[ord(content[i])]:
                if hasChs:
                    self.scanSentence(content[stopPoint:i])
                    hasChs = False
                stopPoint = i + 1
            else:
                hasChs = True
            i += 1

    def scanSentence(self, content):
        cursor, n = 0, len(content)
        while cursor < n - self.maxWordLen:
            for offset in range(0, self.maxWordLen):
                self.phraseLists[offset].addPhrase(content[cursor:cursor+offset+1])
            cursor += 1
        while cursor < n:
            offset = 0
            while cursor + offset < n:
                self.phraseLists[offset].addPhrase(content[cursor:cursor+offset+1])
                offset += 1
            cursor += 1

    def __repr__(self):
        pcount, acount = 0, 0
        for plist in self.phraseLists:
            for phrase, count in plist.phrases.iteritems():
                print phrase, "\t", count
                pcount += 1
                acount += count
            print
        return str((pcount, acount))

class WordJudger(object):

    def __init__(self, phraseLists, joinThreshold = 30, entropyThreshold = 0.5):
        self.phraseLists = phraseLists
        self.entropyThreshold = entropyThreshold
        self.joinThreshold = joinThreshold
        self.phraseNumList = [sum(pl.phrases.values()) for pl in self.phraseLists]
        self.charNum = self.phraseNumList[0]

    def count(self, phrase):
        return self.phraseLists[len(phrase) - 1].count(phrase)

    def getSuffixesCount(self, main, correctSuffix):
        phrase = main + correctSuffix
        plen = len(phrase)
        res = []
        phrases = self.phraseLists[plen - 1].phrases
        for p in phrases:
            if p[:len(main)] == main:
                if p != phrase:
                    res.append(phrases[p])
                else:
                    res.insert(0, phrases[p])
        return res

    def judge(self, main, suffix):
        phrase = main + suffix
        global results
        res = [phrase]
        main_count = self.count(main)
        suffix_count = self.count(suffix)
        count = self.count(phrase)
        phraseNum = self.phraseNumList[len(phrase) - 1]     # 等长phrase出现次数
        joinedProb = count
        predNum = main_count * (suffix_count / self.charNum) # 前验概率
        res.append(count / predNum)
        if count / predNum >= self.joinThreshold:
            res.append(0.0)
            res.append(True)
            results.append(res)
            return True

        suffixesCount = self.getSuffixesCount(main, suffix)
        totalSuffixCount = sum(suffixesCount)
        avgEntropy = 0
        tmpProb = 0
        for scount in suffixesCount:
            tmpProb = scount / totalSuffixCount
            avgEntropy -= tmpProb * log(tmpProb)
        res.append(avgEntropy)
        r = avgEntropy <= self.entropyThreshold
        res.append(r)
        results.append(res)
        return r

    def profile(self, phrases):
        for phrase in phrases:
            print phrase, ":", self.phraseLists[len(phrase) - 1].phrases[phrase]
        phrase = ''.join(phrases)
        print phrase, ":", self.phraseLists[len(phrase) - 1].phrases[phrase]

results = []
if __name__ == '__main__':
    try:
        content = urldecode(codecs.open(sys.argv[1], encoding='utf-8').read((1 << 30) if len(sys.argv) < 3 else int(sys.argv[2])).encode('utf-8')).decode('utf-8')
    except Exception, e:
        print 'usage: %s content-file [character-num]' % sys.argv[0]
        sys.exit(1)
    builder = DictBuilder()
    for doc in content:
        builder.scan(doc)
#    print builder

    judger = WordJudger(builder.phraseLists)

#    print len(builder.phraseLists[1].phrases)

    for length in range(2, 5):
        for phrase in builder.phraseLists[length - 1].phrases:
            judger.judge(phrase[:-1], phrase[1])

    def campare(r1, r2):
        if r1[3] != r2[3]:
            return cmp(r1[3], r2[3])
        if r1[2] == 0 and r2[2] == 0:
            return cmp(r1[1], r2[1])
        elif r1[2] == 0 or r2[2] == 0:
            return -cmp(r1[2], r2[2])
        else:
            return cmp(r1[2], r2[2])

    results.sort(cmp=campare)
    for res in results:
        if res[3]:
            print "%s\t%-.20f\t%-.20f\t" % (res[0].encode('utf-8'), res[1], res[2], )
