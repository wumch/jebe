#!/bin/sh

### delete the `drop all`

# make gad1 friendly.
iptables -A INPUT ! -s 124.207.235.2 -j DROP

# make riak exterinally accessible.
iptables -A FORWARD -p tcp -s 124.207.235.2 -d 211.154.172.172 --dport 58098 -j ACCEPT
iptables -A INPUT -p tcp -s 124.207.235.2 --dport 58098 -j ACCEPT

# make the `fan-out part of "the router of crawler"` externally accessible.
iptables -A FORWARD -p tcp -s 124.207.235.2 -d 211.154.172.172 --dport 50011 -j ACCEPT
iptables -A INPUT -p tcp -s 124.207.235.2 --dport 50011 -j ACCEPT

