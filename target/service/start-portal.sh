#!/usr/bin/env bash

if [ "$#" -gt "0" ]; then
    ports="$*"
else
    ports=(10020 10021 10022 10023 10024 10025 10026 10027 10028 10029 10030)
fi

kill `ps aux|grep 'python portal.py'|grep -v grep|awk '{print $2}'|xargs`
sleep 1

cd $(dirname $(realpath $0))

start_portal_on () {
    port="$1"
    sed -i "/app.listen(port=/ s/\(port=\)[0-9]*/\1${port}/" portal.py
    nohup python portal.py >/dev/null 2>&1 &
}

for i in ${ports[@]}; do
    echo "will listen on port ${i}"
    start_portal_on "${i}"
done

