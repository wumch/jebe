#!/bin/bash

#
# rc-script for service `tcmd`
#

PROGRAM_NAME="fts"
USER="$(whoami)"
OUTLOG=/var/log/${PROGRAM_NAME}.output.log
ERRLOG=/var/log/${PROGRAM_NAME}.error.log
pidfile=/var/run/${PROGRAM_NAME}.pid

ROOT_PATH="$(dirname $(realpath ${0}))"
ROOT_PATH=${ROOT_PATH%/bin}
if [ ! -n "${ROOT_PATH}" ]; then
    echo -e "\033[32;31;5mcan not resolve home directory for tokenizer! maybe you do not have 'realpath'.\033[0m"
    exit 2
fi

cd ${ROOT_PATH}
TCMD_EXEC="bin/fts"

if [ -n "${pidfile}" ]; then
    pidfile=$(awk -F'\\s*=\\s*' '/pid-file *\=/ {pidf=$2; sub(/^[^\/]+/, "", pidf); sub(/[^\w\d]+$/, "", pidf); print pidf;}' "${ROOT_PATH}/etc/fts.conf")
fi

if [ ! -n "${pidfile}" ]; then
    echo -e "\033[32;31;5mcannot resolve pidfile for ${PROGRAM_NAME}!\033[0m"
    exit 1
fi

zstart() {
    if [ -n "$(zgetpid)" ];then
        printf "\033[32;31;5${PROGRAM_NAME} malready started!\033[0m\n"
        return
    fi
    local res=$(zstartup)
    if [ "$?" -eq "0" ]; then
        printf "\033[32;49;5m${PROGRAM_NAME} started!\033[0m\n"
    else
        printf "\033[32;31;5mstart ${PROGRAM_NAME} faild!\033[0m\nerror:\n%s\n" "${res}"
    fi
}

zstop() {
    local pid=$(zgetpid)
    if [ -n "${pid}" ]; then
        kill "${pid}"
        if [ "$?" -eq "0" ]; then
            >"${pidfile}"
            printf "\033[32;495m${PROGRAM_NAME} stoped!\033[0m\n"
        else
            printf "\033[32;31;5mfaild on stop ${PROGRAM_NAME}!\033[0m\n"
        fi
    else
        printf "\033[32;31;5m${PROGRAM_NAME} not yet started!\033[0m\n"
    fi       
}

zrestart () {
    zstop
    zstart
    return $?
}

zstatus () {
    if [ -n "$(zgetpid)" ]; then
        printf "\033[32;49;5m${PROGRAM_NAME} is running!\033[0m\n"
        return 0
    else
        printf "\033[32;31;5m${PROGRAM_NAME} is stoped!\033[0m\n"
        return 1
    fi
}

zgetpid () {
    if [ -r "${pidfile}" ]; then
        local pid=$(cat "${pidfile}")
        if [ -n "${pid}" ]; then
            if [ -d "/proc/${pid}" ]; then
                echo "${pid}"
            else
                >"${pidfile}"
                echo ""
            fi
            return
        fi
    fi
}

zstartup () {
    local cmd="${TCMD_EXEC}"
    zsuexec "${cmd}"
    return $?
}

zsuexec () {
    local cmd="$1" res=""
    if [ ! -n "${cmd}" ]; then
        return
    fi
    if [ -n "${USER}" -a "${USER}" != "$(whoami)" ]; then
        su "${USER}" -c "nohup ${cmd} >>"${OUTLOG}" 2>>"${ERRLOG}" &"
    else
        nohup ${cmd} >>"${OUTLOG}" 2>>"${ERRLOG}" &
    fi
    echo ${res}
    return $?
}

usage () {
    echo -e "usage: $0 \033[32;49;5m[status|start|stop|restart]\033[0m"
    echo -e "       default action is \033[32;49;5mstatus\033[0m"
}


if [ "$#" -eq "0" ]; then
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

