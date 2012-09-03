#!/bin/sh

tail -n 1000 /var/log/nginx/target | awk -F'"' '{status[$2]+=1;if($3==19){crawlOrEmpty+=1}else{shown+=1}}END{print "shown: "shown"\ncrawl-or-empty-ads: "crawlOrEmpty"\n"; for (i in status){print "response-stats["i"]: "status[i]}}'
