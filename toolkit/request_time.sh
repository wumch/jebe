#!/usr/bin/env bash

tail -n 10000 /dev/shm/nginx_target.log | awk '{t=($7<1)?(int($7*100)/100):int($7/5+1)*5; arr[t]+=1}END{for (i in arr){print i"s\t"arr[i]}}' | sort -n -k 1

