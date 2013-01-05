#!/bin/sh

servers=(
    "192.168.88.1"
    "192.168.88.3"
    "192.168.88.6"
)

for server in "${servers[@]}";
do
    ssh $server 'cd $(dirname $(realpath $0))  killall -9 uwsgi ; uwsgi -s 127.0.0.1:9001 -w crawler -p 10 -t 5 -M --limit-as 384 -R 1000 --post-buffering --post-buffering-size 102400 -d /dev/null'
done

