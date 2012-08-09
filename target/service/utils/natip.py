#encoding:utf-8

import os
import socket
import fcntl
import struct
 
def get_ip_address(ifname):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(fcntl.ioctl(
        s.fileno(),
        0x8915,  # SIOCGIFADDR
        struct.pack('256s', ifname[:15])
    )[20:24])

try:
    natip = get_ip_address(r'eth0')
except Exception, e:
    natip = r'127.0.0.1'

