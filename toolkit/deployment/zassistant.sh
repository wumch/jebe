#!/bin/sh

. ./zconfig.sh      # test only

#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< common members <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
declare -i Z_SUCCESS=0
declare -i Z_FAIL=1
notifier_size=8
zocred="\033[32;31;5m%-${notifier_size}s\033[0m"
zocgreen="\033[32;49;5m%-${notifier_size}s\033[0m"
zocblue="\033[32;34;5m%-${notifier_size}s\033[0m"
declare -a zerrors=()

zpaint() {
    if [ ! $# -gt 1 -a -n "${1}" ]; then
        echo "wrong use of zpaint"
        exit ${Z_FAIL}
    fi
    local format="${1}"
    shift
    printf "${format}" "${@}"
}

zcored() {
    zpaint "${zocred}\n" "${@}"
}

zcoblue() {
    zpaint "${zocblue}\n" "${@}"
}

zcogreen() {
    zpaint "${zocgreen}\n" "${@}"
}

zdie() {
    if [ ${#zerrors[@]} -gt 0 ]; then
        local err
        for err in "${zerrors[@]}"; do
            zpaint "${zocblue}" "Error:"
            zcored "${err}"
        done
    fi
    if [ $# -gt 0 ]; then
        zpaint "${zocblue}" "Die:"
        zcored "${@}"
    fi
    exit ${Z_FAIL}
}

zgen_status() {
    ${@} && echo ${Z_SUCCESS} || echo ${Z_FAIL}
}

zpush_error() {
    if [ -n "$@" ]; then
        zerrors[${#zerrors[@]}]="$@"
    fi
}

zpid_exists() {
    test $# -eq 1 -a -n "${1}" && return $(zgen_status "test -d /proc/${1}") || return ${Z_FAIL}
}

zget_listening() {
    test $# -eq 1 -a -n "${1}" || zdie "empty pid"
    netstat -nlp | awk '/'${1}'\// {if ($1=="unix"){print $NF}else{print $4}}'
}

zuser_exists() {
    test $# -eq 1 -a -n "${1}" || return ${Z_FAIL}
    if [ -n "$(grep ${1} /etc/passwd)" ]; then
        return ${Z_SUCCESS}
    else
        return ${Z_FAIL}
    fi
}

zcheck_depends() {
    local result=${Z_SUCCESS}
    local util
    for util in "${@}"; do
        if [ ! -x "$(which ${util} 2>/dev/null)" ]; then
            zpush_error "command '${util}' is required and must be execute-able."
            result=${Z_FAIL}
        fi
    done
    return ${result}
}
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#<<<<<<<<<<<<<<<<<<<<<<<<<<<< app-related functions <<<<<<<<<<<<<<<<<<<<<<<<<<<<
zapp_exists() {
    test $# -eq 1 -a -n "${1}" && return $(zgen_status "test -d ${approot}") || return ${Z_FAIL}
}

zapp_load_config() {
    test -e "${1}" || zdie "${appname}: config file ${1} does not exists!"
    . "${1}"
}

zvalidate_appname() {
    test $# -eq 1 -a -n "${1}" || return ${Z_FAIL}
    local name
    for name in "${RESERVED_APP_NAME[@]}";
    do
        if [ "${name}" == "${1}" ]; then
            return ${Z_FAIL}
        fi
    done
    return ${Z_SUCCESS}
}

zcheck_config() {
    if ! zvalidate_appname "${appname}"; then
        zpush_error "invaild appname '${appname}'"
        return ${Z_FAIL}
    fi
    if zuser_exists "${username}"; then
        zpush_error "username '${username}' already exists!"
        return ${Z_FAIL}
    fi
    return ${Z_SUCCESS}
}

zcreate_user() {
    test $# -eq 1 -a -n "${1}" || zdie "username is empty!"
    useradd                         \
        --create-home               \
        --no-user-group             \
        --gid ${usergroup}          \
        --home-dir ${userhome}      \
        --password ''               \
        --shell /bin/bash           \
        ${username}                 \
    || return ${Z_FAIL}
    passwd --lock "${username}"
    chsh -s /bin/false "${username}"
    return ${Z_SUCCESS}
}

zdelete_user() {
    if ! zuser_exists ${username}; then
        zpush_error "user '${username}' non-exists while 'zdelete_user'"
        return ${Z_FAIL}
    fi
    userdel --force --remove ${username} && return ${Z_SUCCESS} || return ${Z_FAIL}
}
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>


#<<<<<<<<<<<<<<<<<<<<<<<<<<<< run-related functions <<<<<<<<<<<<<<<<<<<<<<<<<<<<
zgetpid() {
    declare -i pid
    if [ $# -ge 1 ]; then
        if [ -f "${1}" ]; then
            pid="$(cat ${1})"
        else
            pid="${1}"
        fi
    elif [ -f "${pidfile}" ]; then
        local pid="$(cat ${1})"
    fi
    echo "${pid}"
    test -n "${pid}" -a -d /proc/${pid} && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zis_running_pid() {
    test -n "${1}" -a -d "/proc/${1}" && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zis_running() {
    declare pid=$(zgetpid "${@}")
    zis_running_pid "${pid}" && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zrun() {
    local old_ulimits=$(ulimit -a | awk '{required="'${ulimits}'"; name_field=NF-1; name=$name_field; val=$NF; gsub(/[\(|\)]/, "", name); if (match(required, name)) {print name" "val}}' | xargs)
    if [ -n "${old_ulimits}" ]; then
        if ! ulimit ${old_ulimits}; then
            zdie "failed to do 'ulimit ${old_ulimits}'"
        fi
        if [ -n "${ulimits}" ]; then
            if ! ulimit "${ulimits}"; then
                zpush_error "failed on ulimit '${ulimits}'"
                return ${Z_FAIL}
            fi
        fi
    fi
    start-stop-daemon --start               \
        --background                        \
        --make-pidfile                      \
        --pidfile=${pidfile}                \
        --chuid ${username}:${usergroup}    \
        --chdir ${approot}                  \
        --nicelevel ${niceincr}             \
        --stdout ${appstdout}               \
        --stderr ${appstderr}               \
        --exec "${@}"                       \
    || return ${Z_FAIL}
    ulimit ${old_ulimits}
    if [ -n "${niceincr}" ]; then
        declare -i pid=$(zgetpid)
        if zis_running_pid "${pid}"; then
            renice -n ${niceincr} -p ${pid}
        fi
    fi
    return ${Z_SUCCESS}
}

zfinish() {
    declare -i pid="$(zgetpid)"
    local listening
    if [ -n "${pid}" ]; then
        listening=$(zget_listening "${pid}")
    fi
    start-stop-daemon --stop                \
        --pidfile=${pidfile}                \
        --retry TERM/3/KILL/3               \
    || return ${Z_FAIL}
    if [ -S "${listening}" ]; then
        if ! rm -f "${listening}" 2>/dev/null; then
            zpush_error "failed on rm sock-file '${listening}'"
            return ${Z_FAIL}
        fi
    fi
    return ${Z_SUCCESS}
}

zinstall() {
    zapp_exists ${appname} && return ${Z_SUCCESS}
    if ! zuser_exists ${username}; then
        zcreate_user || zdie "failed on creating user '${username}'"
    fi
    ${app_install_command} || zdie "failed on installing app '${appname}', command: ${app_install_command}"
    chown -R ${username}:${usergroup} ${approot} || zdie "failed on chown for app '${appname}'"
    return ${Z_SUCCESS}
}

zdelete_app() {
    if [ -d "${approot}" ]; then
        rm -fr ${approot} || zdie
    fi
    return ${Z_SUCCESS}
}

zadjust() {
    local pid=$(zgetpid || echo "")
    if [ -n "${pid}" ]; then
        zget_listening "${pid}"
    fi
    return ${Z_SUCCESS}
}

zstop() {
    zis_running || return ${Z_SUCCESS}      # bug...
    zfinish || zdie "failed on stopping '${appname}'"
    zdelete_app || zdie "failed on remove app '${appname}'"
    zdelete_user ${username} || zdie "failed on deleting user '${username}'"
    return ${Z_SUCCESS}
}

zstart() {
    zis_running && return ${Z_SUCCESS}      # bug...
    zinstall || zdie
    zrun || zdie
    zadjust || zdie
    return ${Z_SUCCESS}
}

zrestart() {
    zstop && zstart || return ${Z_FAIL}
    return ${Z_SUCCESS}
}

zinitsys() {
    declare -a directories=("${APPS_ROOT}" "${APPS_LOG_DIR}" "${APP_USER_HOME_ROOT}" "${APPS_PIDFILE_DIR}")
    local directory
    for directory in "${directories[@]}";
    do
        if [ ! -d "${directory}" ]; then
            mkdir -p "${directory}" && chgrp ${usergroup} "${directory}" || zdie "failed on 'mkdir -p ${directory}'"
        fi
    done
    local err=$(groupadd "${usergroup}" 2>&1)
    if [ -n "${err}" ]; then
        zpush_error "${err}"
    fi
    groupmems -g ${usergroup} -a $(whoami) >/dev/null 2>&1
    if [ -n "${FIFO_NAME}" ]; then
        if [ ! -p "${FIFO_NAME}" ]; then
            mkfifo ${FIFO_NAME} || zdie "failed on 'mkfifo ${FIFO_NAME}'"
            chgrp ${usergroup} ${FIFO_NAME}
            chmod o-rw,g+r ${FIFO_NAME}
        fi
        if [ -p "${FIFO_NAME}" ]; then
            nohup ${COMMAND_WATCH_FIFO} < ${FIFO_NAME} >/dev/null 2>&1 &
        fi
    fi
}
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
