#!/usr/bin/env python
#coding:utf-8

import pymongo
import time
import math

class RelImporter(object):

    def __init__(self, relfile, limit=None):
        self.relfile = relfile
        self._prepare()
        self.limit = limit

    def _prepare(self):
        self.mongoCon = pymongo.Connection(host="192.168.88.8")
        self.mongo = self.mongoCon["words-rel-new"]["main"]
        self.fp = open(self.relfile, "r")

    def run(self):
        self._import_pair()

    def _import_pair(self):
        count = 0
        step = 100000
        straight = 100
        docs = []
        for line in self.fp:
            Wa, Wb, C = [f.strip() for f in line.split("\t")]
            C = float(C)
            docs.append({'_id':Wa+'_'+Wb, 'w':C})
            docs.append({'_id':Wb+'_'+Wa, 'w':C})
            count += 2
            if count >= straight:
                self._sotre_pair(docs)
                docs = []
                straight = count + 100
                print "imported", count
        self._sotre_pair(docs)
        if count > step:
            step = math.ceil(count / 100000.0) * 100000
            time.sleep(0.2)

    def _import_one_to_multi(self):
        relsmap = {}
        count = 0
        step = 100000
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
            count += 2
        if self.limit is None or count <= self.limit:
            for word, rels in relsmap.iteritems():
                self._sotre(word, rels)
        if count > step:
            step = math.ceil(count / 100000.0) * 100000
            print "imported", count
            time.sleep(0.2)

    def _sotre_pair(self, docs):
        if docs:
            self.mongo.insert(docs)

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
    limit = None if len(sys.argv) < 3 else int(sys.argv[2])
    RelImporter(sys.argv[1], limit=limit).run()
