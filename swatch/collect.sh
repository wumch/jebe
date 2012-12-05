#!/bin/bash

logfile=${1:-/dev/shm/crawler.log}
hostname="$(hostname -i)"
for line in $(grep -Ev '^\s*#' /code/cluster-control/servers.list)
do
	if [ "${line}" != "${hostname%% *}" ]; then
        nohup ssh ${line} "tail -f /var/log/nginx/ad.i8ad.cn.access.log" >> ${logfile} 2>/dev/null &
    else
        nohup tail -f /var/log/nginx/ad.i8ad.cn.access.log >> ${logfile} 2>/dev/null &
	fi
done 

