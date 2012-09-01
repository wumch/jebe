#coding:utf-8

from hashlib import md5 as MD5

def md5(string):
    return MD5(string.encode('utf-8') if isinstance(string, unicode) else string).hexdigest()

def md516(string):
    return MD5(string.encode('utf-8') if isinstance(string, unicode) else string).digest()

MOVE_KEY_HYPHEN = '-'
reserve_chars = (MOVE_KEY_HYPHEN, )
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

def export(var):
    if isinstance(var, basestring):
        print(var)
        return
    if isinstance(var, dict):
        for k, v in var.iteritems():
            print k, ':',
            export(v)
        return
    if isinstance(var, (list, tuple, set)):
        for v in var:
            export(v)
        return
    # if missed all of patterns specified above:
    print var

m_pdwCrc32Table = [0] * 256
dwPolynomial = 0xEDB88320
for i in xrange(0,255):
    dwCrc = i
    for j in [8,7,6,5,4,3,2,1]:
        if dwCrc & 1:
            dwCrc = (dwCrc >> 1) ^ dwPolynomial
        else:
            dwCrc >>= 1
    m_pdwCrc32Table[i] = dwCrc

def crc32(szString):
    dwCrc32 = 0xFFFFFFFFL
    for i in (szString.encode('utf-8') if isinstance(szString, unicode) else (szString if isinstance(szString, str) else str(szString))):
        dwCrc32 = (dwCrc32 >> 8) ^ m_pdwCrc32Table[ord(i) ^ (dwCrc32 & 0x000000FF)]
    return dwCrc32 ^ 0xFFFFFFFFL