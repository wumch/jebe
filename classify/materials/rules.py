#coding:utf-8

import re

NOT = 0
MAY_BE = 1
MUST_BE = 2
SURE = 3

class NotImplementError(Exception):
    def __init__(self, *args, **kwargs):
        super(Exception, self).__init__(*args, **kwargs)

class Operation(object):

    def __init__(self):
        self.cached_regexp = {}

    def eq(self, property, consult, confidence=SURE):
        return confidence if property == consult else NOT

    def neq(self, property, consult, confidence=SURE):
        return confidence if property != consult else NOT

    def match(self, property, consult, confidence=SURE):
        if isinstance(consult, basestring):
            if consult not in self.cached_regexp:
                consult = self.cached_regexp[consult] = re.compile(consult)
        return confidence if consult.search(property) else NOT

    def unmatch(self, property, consult, confidence=SURE):
        return NOT if self.match(property=property, consult=consult) else confidence

    def endswith(self, property, consult, confidence=SURE):
        return confidence if property.endswith(consult) else NOT

    def startswith(self, property, consult, confidence=SURE):
        return confidence if property.startswith(consult) else NOT

    def contains(self, property, consult, confidence=SURE):
        return confidence if consult in property else NOT

    def inside(self, property, consult, confidence=SURE):
        return confidence if property in consult else NOT

    def notinside(self, property, consult, confidence=SURE):
        return NOT if property in consult else confidence

    def uncontains(self, property, consult, confidence=SURE):
        return NOT if consult in property else confidence

class Predicate(object):

    def __init__(self, attr, op, consult, confidence=SURE):
        self.attr = attr
        self.op = op
        self.consult = consult
        self.result = confidence
        self.next_and = None
        self.next_or = None
        self.up_op = None

    def _evaluate(self, property, consult):
        return self.op(property, consult) and self.result

    def evaluate(self, subject, consult):
        curres = self._evaluate(property=self.attr.pump(subject), consult=consult)
        if self.next_and:
            return curres if curres == NOT else min(curres, self.next_and(subject=subject))
        elif self.next_or:
            return curres if curres == SURE else max(curres, self.next_or(subject=subject))
        return curres

    def __call__(self, subject):
        return self.evaluate(subject=subject, consult=self.consult)

    def __and__(self, other):
        self.next_and = other
        other.up_op = self
        return other

    def __or__(self, other):
        self.next_or = other
        other.up_op = self
        return other

class Attr(object):

    def __init__(self):
        self.operation = Operation()
        self.ops = {
            'eq':self.operation.eq,
            'neq':self.operation.neq,
            'match' : self.operation.match,
            'unmatch':self.operation.unmatch,
            'endswith':self.operation.endswith,
            'startswith':self.operation.startswith,
            'contains':self.operation.contains,
            'uncontains':self.operation.uncontains,
            'inside':self.operation.inside,
            'notinside':self.operation.notinside,
        }

    def pump(self, subject):
        raise NotImplementError()

    def __getattr__(self, item):
        return lambda consult,confidence=SURE: Predicate(attr=self, op=self.ops[item], consult=consult, confidence=confidence)

class AttrPumper(Attr):

    def __init__(self, property_key):
        super(AttrPumper, self).__init__()
        self.property_key = property_key

    def pump(self, subject):
        return subject.get(self.property_key)

class PredicateList(object):

    def __init__(self, *args, **kwargs):
        self.min_confidence = kwargs['min_confidence'] if 'min_confidence' in kwargs else SURE
        self.predicates = args

    def __call__(self, subject):
        for pred in self.predicates:
            p = pred
            while p.up_op:
                p = p.up_op
            if p(subject=subject) >= self.min_confidence:
                return True
        return False

if __name__ == '__main__':

    # field name:
    DOMAIN = 'domain'
    MAIN_DOMAIN = 'maindomain'
    SUB_DOMAIN = 'subdomain'
    TOP_PATH = 'toppath'

    domain = AttrPumper(DOMAIN)
    maindomain = AttrPumper(MAIN_DOMAIN)
    subdomain = AttrPumper(SUB_DOMAIN)
    toppath = AttrPumper(TOP_PATH)

    s = {DOMAIN:"www.baidu.com", SUB_DOMAIN:"sss"}
    r = domain.eq("www.baidu.com") & subdomain.match('www') & domain.startswith('www.baiduw')
    print r(s)
