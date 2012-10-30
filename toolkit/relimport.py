#!/usr/bin/env python
#coding:utf-8

import pymongo

class RelImporter(object):

    def __init__(self, relfile):
        self.relfile = relfile
        self._prepare()

    def _prepare(self):
        self.mongoCon = pymongo.Connection(host="192.168.88.8")
        self.mongo = self.mongoCon["words-rel"]["main"]
        self.fp = open(self.relfile, "r")

    def run(self):
        relsmap = {}
        for line in self.fp:
            Wa, Wb, C = [f.strip() for f in line.split("\t")]
            rel = (Wb, float(C))
            if Wa in relsmap:
                relsmap[Wa].append(rel)
            else:
                relsmap[Wa] = [rel]
            rel2 = (Wa, float(C))
            if Wb in relsmap:
                relsmap[Wb].append(rel2)
            else:
                relsmap[Wb] = [rel2]
        for word, rels in relsmap.iteritems():
            self._sotre(word, rels)

    def _sotre(self, Wa, rels):
        if not Wa or not rels:
            return
        rels.sort(lambda a, b: -cmp(a[1], b[1]))
        entry = {
            '_id' : Wa,
            'rels' : [r[0] for r in rels],
#            'sims' : [r[1] for r in rels],
        }
        self.mongo.insert(entry)

if __name__ == '__main__':
    import sys
    if len(sys.argv) < 2:
        print "usage: %s <words-relation-file>" % sys.argv[0]
        sys.exit(1)
    RelImporter(sys.argv[1]).run()
