#!/usr/bin/env bash

if [ "$#" -gt "0" ]; then
    ports="$*"
else
    ports=(10030 10031 10032 10033)
fi

#pids=`ps aux|grep 'python[0-9\.]* \(portal.py\|python dealer.py\)'|grep -v grep|awk '{print $2}'|xargs`
#if [ -n "${pids}" ]; then
#    kill ${pids}
#fi
#sleep 0.3

cd $(dirname $(realpath $0))

restart_dealer() {
    local pids=`ps aux|grep "python[0-9\.]* *dealer.py"|grep -v grep|awk '{print $2}'|xargs`
    if [ -n "${pids}" ]; then
        echo "kill dealers: ${pids}"
        kill ${pids}
        sleep 0.1 
    fi  
    for ((i=0;i<3;++i)); do
        echo "starting dealer ${i}"
        nohup python dealer.py >/dev/null 2>&1 &
        sleep 0.2
    done
}

start_portal_on() {
    local port="$1"
    local pid=`ps aux|grep "python[0-9\.]* *\(portal.py\|dealer.py\) *${port}"|grep -v grep|awk '{print $2}'|xargs`
    if [ -n "${pid}" ]; then
        echo "kill portal: ${pid}"
        kill ${pid}
        sleep 0.1
    fi
    nohup python portal.py "${port}" >/dev/null 2>&1 &
}

restart_dealer
for i in ${ports[@]}; do
    echo "will listen on port ${i}"
    start_portal_on "${i}"
    sleep 0.2
done

