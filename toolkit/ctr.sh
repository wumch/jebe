#!/usr/bin/env sh

shown=$(grep 'ad shown:' /var/log/target-server.log | grep -v 坦克世界 | wc -l)
clicked=$(grep -v '?ad=30' /var/log/nginx/adclick | awk '{if ($9!="124.207.235.2"&&($9!=last["ip"]||$12!=last["ad"])){last["ip"]=$9;last["ad"]=$12;total+=1;}else{last["ip"]="ie6"}}END{print total}')
echo "shown: ${shown}, clicked: ${clicked}"
printf "ctr: 0.%04d%%\n" $((clicked*1000000/shown))

