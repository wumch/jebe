#!/bin/sh

### delete the `drop all`

# make gad1 friendly.

# make riak exterinally accessible.
iptables -t nat -A PREROUTING -p tcp -s 124.207.235.2 -d 211.154.172.172 --dport 58098 -j DNAT --to 192.168.88.1:8098
iptables -A FORWARD -p tcp -s 124.207.235.2 -d 211.154.172.172 --dport 58098 -j ACCEPT
iptables -A INPUT -p tcp -s 124.207.235.2 --dport 58098 -j ACCEPT

# make the `fan-out part of "the router of crawler"` externally accessible.
iptables -t nat -A PREROUTING -p tcp -s 124.207.235.2 -d 211.154.172.172 --dport 50011 -j DNAT --to 192.168.88.1:10011
iptables -A FORWARD -p tcp -s 124.207.235.2 -d 211.154.172.172 --dport 50011 -j ACCEPT
iptables -A INPUT -p tcp -s 124.207.235.2 --dport 50011 -j ACCEPT

# sometimes...
iptables -A INPUT ! -s 124.207.235.2 -j DROP
iptables -A INPUT -j DROP
iptables -A FORWARD -j DROP

