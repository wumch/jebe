#coding:utf-8
import os, sys

class IDFAnalysis(object):

    def __init__(self, inputfile, delimiter=(0.5,), before=50, after=50):
        self.inputfile = inputfile
        self.delimiter = delimiter
        self.before = before
        self.after = after

    def run(self):
        pairs = []
        total = .0
        for line in open(self.inputfile, 'r'):
            pair = line.split('\t')
            pair[1] = float(pair[1])
            if 7.834880 > pair[1] > 0:
                total += pair[1]
                pairs.append(pair)
        pairs.sort(lambda a,b: cmp(b[1], a[1]))
        self.out(sorted_pairs=pairs)
        self.analysis(pairs=pairs, total=total)

    def analysis(self, pairs, total):
        e = total / len(pairs)
        d = sum(map(lambda p: (p[1] - e) ** 2, pairs)) / len(pairs)
        print os.linesep, 'e:', e
        print 'd:', d

    def out(self, sorted_pairs):
        upper = len(sorted_pairs) - 1
        for delimiter in self.delimiter:
            print '<' + '-' * 30, delimiter, '-' * 30
            point = min(int(delimiter * upper), upper)
            for i in range(max(0, point - self.before), point):
                print "%s\t%f" % tuple(sorted_pairs[i])
            print '-' * 10, delimiter, ':', sorted_pairs[point][0], ',', '%f' % sorted_pairs[point][1], '-' * 10
            for i in range(point, min(upper, point + self.after)):
                print "%s\t%f" % tuple(sorted_pairs[i])
            print '-' * 30, delimiter, '-' * 30 + '>', os.linesep

if __name__ == '__main__':
    if len(sys.argv) < 2:
        print "usage: %s <input-file> [delimiter=0.5] [before] [after]" % sys.argv[0]
        sys.exit(0)
    inputfile = sys.argv[1]
    delimiter = [0.1 * i for i in range(0, 11)] if len(sys.argv) < 3 else [float(sys.argv[2])]
    before = 20 if len(sys.argv) < 4 else int(sys.argv[4])
    after = 20 if len(sys.argv) < 5 else int(sys.argv[4])
#    delimiter[0] = 0.05
#    delimiter[9] = 0.95
    delimiter = [0.99999, 0.99998, 0.99997,0.99996, 0.3,0.25,0.2,0.15]
    analyzer = IDFAnalysis(
        inputfile=inputfile,
        delimiter=delimiter,
        before=before,
        after=after
    )
    analyzer.run()
