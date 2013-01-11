#!/bin/sh

#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< common members <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
declare -i Z_SUCCESS=0
declare -i Z_FAIL=1
notifier_size=8
zocred="\033[32;31;5m%-${notifier_size}s\033[0m"
zocgreen="\033[32;49;5m%-${notifier_size}s\033[0m"
zocblue="\033[32;34;5m%-${notifier_size}s\033[0m"
declare -a zerrors=()

zpaint() {
    if [ ! $# -ge 1 -a -n "${1}" ]; then
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

zcogreen() {
    zpaint "${zocgreen}\n" "${@}"
}

zcoblue() {
    zpaint "${zocblue}\n" "${@}"
}

zemit_errors() {
    local prefix=${1:-""}
    if [ ${#zerrors[@]} -gt 0 ]; then
        local err
        for err in "${zerrors[@]}"; do
            if [ -n "${prefix}" ]; then
                zpaint "${zocblue}" "${prefix}"
            fi
            zcored "${err}"
        done
    fi
}

zdie() {
    zemit_errors "Error:"
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

zpid_alive() {
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
    return $(zgen_status "test -d ${approot}")
}

zis_appname_valid() {
    test $# -eq 1 -a -n "${1}" || return ${Z_FAIL}
    # appname starts with hyphen/dot is invalid.
    if [ "${1:0:1}" == "-"  -o "${1:0:1}" == "." ]; then
        return ${Z_FAIL}
    fi
    return ${Z_SUCCESS}
}

zis_appname_reserved() {
    test $# -eq 1 -a -n "${1}" || return ${Z_FAIL}
    local name
    for name in "${RESERVED_APP_NAME[@]}";
    do
        if [ "${name}" == "${1}" ]; then
            return ${Z_SUCCESS}
        fi
    done
    return ${Z_FAIL}
}

zcheck_config() {
    if ! zis_appname_valid "${appname}"; then
        zpush_error "format of appname '${appname}' is wrong."
        return ${Z_FAIL}
    fi
    if zis_appname_reserved "${appname}"; then
        zpush_error "invaild appname '${appname}'"
        return ${Z_FAIL}
    fi
    return ${Z_SUCCESS}
}

zcreate_user() {
    test $# -eq 1 -a -n "${1}" || zdie "username is empty!"
    if zuser_exists "${1}"; then
        zpush_error "username '${1}' already exists!"
        return ${Z_FAIL}
    fi
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
        --chroot ${appchroot}               \
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
        zcreate_user ${username} || zdie "failed on creating user '${username}'"
    fi
    ${app_install_command} || zdie "failed on installing app '${appname}', command: ${app_install_command}"
    chown -R ${username}:${usergroup} ${approot} || zdie "failed on chown for app '${appname}'"
    return ${Z_SUCCESS}
}

zdelete_app() {
    if [ -d "${approot}" ]; then
        \rm -fr ${approot} || zdie
    fi
    zdelete_user ${username} || zdie "failed on deleting user '${username}'"
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

zmkchroot() {
    test -n "${1}" || return ${Z_FAIL}
    declare -a directories=("/usr/bin" "/lib" "/usr/lib" "/proc" "/var")
    local directory
    for directory in "${directories[@]}";
    do
        local target="${1}/${directory}"
        if [ ! -d  "${target}" ]; then
            mkdir -p "${target}"
        fi
        mount --bind "${directory}" "${target}" || zdie "failed on mounting --bind '${directory}' '${target}'"
    done
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
    zmkchroot && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zcheck_appname() {
    if ! zis_appname_valid ${appname}; then
        zpush_error "format of appname '${appname}' is wrong."
        return ${Z_FAIL}
    fi
    if zis_appname_reserved ${appname}; then
        zpush_error "appname '${appname}' is reserved."
        return ${Z_FAIL}
    fi
    if zuser_exists ${username}; then
        zpush_error "username '${appname}' already exists."
        return ${Z_FAIL}
    fi
    if zapp_exists ${appname}; then
        zpush_error "app '${appname}' already exists."
        return ${Z_FAIL}
    fi
    return ${Z_SUCCESS}
}
#>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

prepare() {
    . ./zconfig.sh
    zcheck_depends "${DEPEND_UTILS[@]}" || zdie
    zcheck_config || zdie "wrong config"
}

usage () {
    echo "usage:"
    printf "    %s %s %s\n" "$(zcoblue $0)" "$(zcogreen '<appname>')" "$(zcogreen '[status|start|stop|restart|install|checkname]')"
    printf "    default action is %s\n" "$(zcogreen 'status')"
    echo -e "or:"
    printf "    %s %s\t\t%s\n" "$(zcoblue $0)" "$(zcogreen 'initsys')"  "# initialize system."
    printf "    %s %s\t\t%s\n" "$(zcoblue $0)" "$(zcogreen '-h,--help')"  "# show this help and exit."
}

status() {
    prepare
    if ! zapp_exists; then
        zcored "app '${appname}' non-exists."
        return ${Z_FAIL}
    fi
    if zis_running; then
        zcogreen "${appname} is runing"
        return ${Z_SUCCESS}
    else
        zcored "${appname} is stopped"
        return ${Z_FAIL}
    fi
}

start() {
    prepare
    if zstart; then
        zcogreen "${appname} started"
        return ${Z_SUCCESS}
    else
        zcored "failed on starting ${appname}"
        return ${Z_FAIL}
    fi
}

stop() {
    prepare
    if zstop; then
        zcogreen "${appname} stopped"
        return ${Z_SUCCESS}
    else
        zcored "failed on stopping ${appname}"
        return ${Z_FAIL}
    fi
}

restart() {
    prepare
    if zrestart; then
        zcogreen "${appname} restarted"
        return ${Z_SUCCESS}
    else
        zcored "failed on restarting ${appname}"
        return ${Z_FAIL}
    fi
}

unknown_act() {
    prepare
    echo -e "bad action: "$(zcored "${1}")
    usage
    exit ${Z_FAIL}
}

install() {
    prepare
    if zinstall; then
        zcogreen "successfully installed ${appname}"
        return ${Z_SUCCESS}
    else
        zcored "failed on installing ${appname}"
        return ${Z_FAIL}
    fi
}

# check current appname is useable (create-able) or not.
checkname() {
    . ./zconfig.sh
    if zcheck_appname; then
        zcogreen "appname '${appname}' is available"
        return ${Z_SUCCESS}
    else
        zemit_errors
        return ${Z_FAIL}
    fi
}

initsys() {
    . ./zconfig.sh
    if zinitsys; then
        zcogreen "system successfully initialized"
        return ${Z_SUCCESS}
    else
        zcored "failed on initializing system"
        return ${Z_FAIL}
    fi
}

# entry of scripts
appname=""
act=""
if [ $# -ge 2 ]; then
    appname="${1}"
    act="${2}"
elif [ $# -eq 1 ]; then
    if [ "${1}" == "initsys" ]; then
        act="${1}"
    else
        usage
        appname="${1}"
        act="status"
    fi
fi

if [ "${act}" != "initsys" ] && [ ! -n "${appname}" -o ! -n "${act}" ]; then
    usage
    exit 1
fi

case "${act}" in
status) status;;
install) install;;
initsys) initsys;;
start)  start;;
stop)   stop;;
restart)    restart;;
checkname) checkname;;
'--help'|'-h') usage;;
*) unknown_act ${act};;
esac
