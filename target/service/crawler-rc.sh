#!/usr/bin/env bash

#
# rc-script for service `tcmd`
#

PROGRAM_NAME="crawler"
USER="$(whoami)"

ROOT_PATH="$(dirname $(dirname $(dirname $(realpath ${0}))))"
if [ ! -d "${ROOT_PATH}" ]; then
    echo -e "\033[32;31;5mcan not resolve home directory for tokenizer! maybe you do not have 'realpath'.\033[0m"
    exit 2
fi

zstart() {
    cws/bin/cws.sh start
    policy/bin/policy.sh start
    router/bin/router.sh start
    dealer/bin/dealer.sh start
}

zstop() {
    cws/bin/cws.sh stop
    policy/bin/policy.sh stop
    router/bin/router.sh stop
    dealer/bin/dealer.sh stop
}

zrestart () {
    zstop
    zstart
    return $?
}

zstatus () {
    cws/bin/cws.sh status
    policy/bin/policy.sh status
    router/bin/router.sh status
    dealer/bin/dealer.sh status
}

usage () {
    echo -e "usage: $0 \033[32;49;5m[status|start|stop|restart]\033[0m"
    echo -e "       default action is \033[32;49;5mstatus\033[0m"
}


if [ "$#" -eq "0" ]; then
    usage
    act="status"
else
    act="$1"
fi

case "${act}" in

status) zstatus;;

start)  zstart;;

stop)   zstop;;

restart)    zrestart;;

repair)     zrepair;;

'--help') usage;;

'-h') usage;;

*)
    printf "bad action: \"\033[32;31;5m%s\033[0m\"\n" "${act}"
    usage
    exit 1
    ;;

esac

