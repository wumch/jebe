#coding:utf-8

from hashlib import md5 as MD5

def md5(string):
    return MD5(string).hexdigest()

def md516(string):
    return MD5(string).digest()

reserve_chars = ('-', )
# base-converter, (riak key shortner)
# NOTE: should not depend on any python features.
numrep = map(str, range(0, 10)) + \
         [chr(i) for i in xrange(ord('a'), ord('z') + 1)] + \
         [chr(i) for i in xrange(ord('A'), ord('Z') + 1)]
usable_chars = filter(lambda x: x not in reserve_chars, [chr(i) for i in xrange(33, 127)])
numrep += tuple(filter(lambda x: x not in numrep, usable_chars))
default_base = len(numrep)
def intenc(num, base = default_base):
    if num < base: return numrep[num]
    res = ''
    while num > 0:
        res = numrep[num % base] + res
        num //= base
    return res

def strenc(string):
    return intenc(int(md5(string)[::2], 16))
