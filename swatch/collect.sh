#!/bin/bash

logfile=${1:-/dev/shm/crawling}
hostname="$(hostname -i)"
for line in $(grep -Ev '^\s*#' /code/cluster-control/servers.list)
do
	if [ "${line}" != "${hostname%% *}" ]; then
        nohup ssh 211.154.172.190 "tail -f /var/log/nginx/ad.i8ad.cn.access.log" >> ${logfile}
    else
        tail -f /var/log/nginx/ad.i8ad.cn.access.log >> ${logfile}
	fi
done 

