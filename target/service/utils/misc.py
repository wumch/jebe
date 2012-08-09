#coding:utf-8

from hashlib import md5 as MD5

def md5(string):
    return MD5(string).hexdigest()

def md516(string):
    return MD5(string).digest()
