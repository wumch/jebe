#!/bin/sh

tail -n 10000 /dev/shm/nginx_target.log | awk -F'"' '{status[$2]+=1;if($3==19){crawlOrEmpty+=1}else{shown+=1}}END{print "shown: "shown"\ncrawl-or-empty-ads: "crawlOrEmpty"\n"; for (i in status){print "response-stats["i"]: "status[i]}}'

