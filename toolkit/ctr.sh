#!/usr/bin/env bash

#shown=$(grep 'ad shown:' /var/log/target-server.log | grep -v 坦克世界 | wc -l)
cmd="grep 'ad shown:' /var/log/adsys.log | wc -l"
declare -i shown=0
shown+=$(ssh 192.168.88.2 "${cmd}")
shown+=$(ssh 192.168.88.3 "${cmd}")
shown+=$(ssh 192.168.88.4 "${cmd}")
clicked=$(awk '{if ($9!="124.207.235.2"&&($9!=last["ip"]||$12!=last["ad"])){last["ip"]=$9;last["ad"]=$12;total+=1;}else{last["ip"]="ie6"}}END{print total}' /var/log/nginx/adclick)
echo "shown: ${shown}, clicked: ${clicked}"
printf "ctr: 0.%04d%%\n" $((clicked*1000000/shown))

