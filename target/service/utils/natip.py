#encoding:utf-8

import sys
import socket, fcntl, struct
 
def get_ip_addr(ifname = 'lo'):
    s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    return socket.inet_ntoa(
        fcntl.ioctl(s.fileno(), 0x8915, struct.pack('256s', ifname[:15]))[20:24]
    )

try:
    natip = get_ip_addr('eth0:1')
except IOError:
    try:
        natip = get_ip_addr('eth0')
    except IOError:
        natip = get_ip_addr()

if __name__ == '__main__':      # for Makefile
    print natip if len(sys.argv) == 1 else get_ip_addr(sys.argv[1])
