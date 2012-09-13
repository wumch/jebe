#!/usr/bin/env bash

if [ "$#" -gt "0" ]; then
    ports="$*"
else
    ports=(10020 10021 10022 10023)
fi

pids=`ps aux|grep 'python[0-9\.]* \(portal.py\|python dealer.py\)'|grep -v grep|awk '{print $2}'|xargs`
if [ -n "${pids}" ]; then
    kill ${pids}
fi
sleep 0.3

cd $(dirname $(realpath $0))

restart_dealer () {
    for ((i=0;i<3;++i)); do
        echo "starting dealer ${i}"
        nohup python dealer.py >/dev/null 2>&1 &
        sleep 0.2
    done
}

start_portal_on () {
    port="$1"
    #sed -i "/addr \= (natip/ s/\(natip\,\s*\)[0-9]*/\1${port}/" portal.py
    nohup python portal.py "${port}" >/dev/null 2>&1 &
}

for i in ${ports[@]}; do
    echo "will listen on port ${i}"
    start_portal_on "${i}"
    sleep 0.2
done

