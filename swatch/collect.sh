#!/bin/bash

declare -a servers=(192.168.88.1 192.168.88.3 192.168.88.6)
logfile=${1:-/dev/shm/crawler.log}
hostname="$(hostname -i)"
for line in "${servers[@]}";
do
	if [ "${line}" != "${hostname%% *}" ]; then
        nohup ssh ${line} "tail -f /var/log/nginx/ad.i8ad.cn.access.log" >> ${logfile} 2>/dev/null &
    else
        nohup tail -f /var/log/nginx/ad.i8ad.cn.access.log >> ${logfile} 2>/dev/null &
	fi
done 

