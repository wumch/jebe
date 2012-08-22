#!/bin/sh

cmd="killall -9 uwsgi ; sleep 1 ; uwsgi -x /code/jebe/target/service/uwsgi.xml"

hostname="$(hostname -i)"
for line in $(cat /code/cluster-control/servers.list)
do
	if [ ! -n "${line}" ]; then
		continue
	fi

	if [ "${line}" = "${hostname%% *}" ]; then
		killall -9 uwsgi
		sleep 1
		uwsgi -x /code/jebe/target/service/uwsgi.xml
	else
		ssh ${line} "${cmd}"
	fi
	
	if [ "$?" = "0" ]; then
		echo "${line} done"
	else
		echo "${line} failed"
	fi
done

