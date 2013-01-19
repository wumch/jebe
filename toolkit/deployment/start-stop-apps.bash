#!/bin/sh

#<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< common members <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
declare -i Z_SUCCESS=0 Z_FAIL=1
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
    netstat -nlp | awk '/\b'${1}'\// {if ($1=="unix"){print $NF}else{sub(/^.*:/, "", $4);print $4}}'
}

zuser_exists() {
    test $# -eq 1 -a -n "${1}" || return ${Z_FAIL}
    test -n "$(grep ${1} /etc/passwd)" && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zcheck_depends() {
    local result=${Z_SUCCESS} util
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
    2>/dev/null || return ${Z_FAIL}
    passwd --lock "${username}" 1>/dev/null
#    chsh -s /bin/false "${username}"
    return ${Z_SUCCESS}
}

zdelete_user() {
    if ! zuser_exists ${username}; then
        zpush_error "user '${username}' non-exists while '$0'"
        return ${Z_FAIL}
    fi
    userdel --force --remove ${username} 2>/dev/null && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zis_dir_empty() {
    # I'd rather die than to rm -fr /
    test -n "${1}" -a -d "${1}" || zdie "argument#1 is wrong"
    declare -r empty_view=". .."
    if [ "$(ls -a ${1} | xargs)" == "${empty_view}" ]; then
        return ${Z_SUCCESS}
    else
        return ${Z_FAIL}
    fi
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
    elif [ -f "${appchroot}${pidfile}" ]; then
        local pid="$(cat ${appchroot}${pidfile})"
    fi
    echo "${pid}"
    zpid_alive "${pid}" && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zis_running_pid() {
    zpid_alive "${1}" && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zis_running() {
    declare pid=$(zgetpid "${@}")
    zis_running_pid "${pid}" && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zrun() {
    local old_ulimits
    if [ -n "${ulimits}" ]; then
        old_ulimits=$(ulimit -a | awk "{required=\"${ulimits}\"; name_field=NF-1; name=\$name_field; val=\$NF; gsub(/[\(\)]/, \"\", name); if (match(required, name)) {print name\" \"val}}")
        if [ -n "${old_ulimits}" ]; then
            if ! ulimit ${old_ulimits}; then
                zdie "failed to reset ulimits, command: 'ulimit ${old_ulimits}'"
            fi
        fi
        if ! ulimit ${ulimits}; then
            zpush_error "failed on ulimit '${ulimits}'"
            return ${Z_FAIL}
        fi
    fi
    local related_path="${approot#${appchroot}}" shell=""
    if [ -n "${related_path}" ]; then
        shell="${shell} cd ${related_path} && ";
    fi
    if [ -n "${ulimits}" ]; then
        shell="${shell} ulimit ${ulimits} && "
    fi
    if [ -n "${app_run_command}" ]; then
        if [ -n "${niceincr}" ]; then
            shell="${shell} nice -n ${niceincr}"
        fi
        shell="${shell} ${app_run_command} >/dev/null 2>/dev/null &"
    fi

    if [ -n "${shell}" ]; then
        local comp_shell=" %s "
        if [ -n "${appchroot}" ]; then
            comp_shell=$(printf "${comp_shell}" "chroot ${appchroot} /bin/bash -c ' %s ' ")
        fi
        if [ -n "${username}" ]; then
            comp_shell=$(printf "${comp_shell}" "su ${username} -c \" %s \" ")
        fi
        comp_shell=$(printf "${comp_shell}" "${shell}")
        if [ -n "${comp_shell}" ]; then
            eval ${comp_shell}
        fi
    fi
    local procflag=$(printf "%s" "${appprocflag}" | sed 's/^-*//g')
    local assistflag=$(printf "%s" "${assistprocflag}" | sed 's/^-*//g')
    declare -a pids=($(ps aux | grep "${procflag}" | grep -v "${assistflag}" | grep -v grep | awk '{print $2}'))
    if [ ${#pids[@]} -ne 1 ]; then
        zdie "failed on picking process-id of '${appname}'"
    fi
    declare -i pid="${pids[0]}"
    if [ ${pid} -eq 0 ]; then
        zdie "pid picked by ps|aux is wrong while starting app '${appname}'"
    fi
    echo ${pid} > ${pidfile}

    if zis_running; then
        return ${Z_SUCCESS}
    else
        return ${Z_FAIL}
    fi
    return ${Z_SUCCESS}
}

zfinish() {
    declare -i pid="$(zgetpid)"
    if [ ! -n "${pid}" ]; then
        return ${Z_SUCCESS}
    fi
    local listening
    if [ -n "${pid}" ]; then
        listening=$(zget_listening "${pid}")
    fi
    declare -i tried_term=0 tried_kill=0 cur_phase=0
    declare -ir MAX_TRY_TERM=3 MAX_TRY_KILL=3
    while zis_running ${pid};
    do
        if [ ${cur_phase} -eq 0 ]; then
            if [ ${tried_term} -lt ${MAX_TRY_TERM} ]; then
                kill -TERM ${pid}
                tried_term+=1
            else
                cur_phase+=1
            fi
        elif [ ${cur_phase} -eq 1 ]; then
            if [ ${tried_kill} -lt ${MAX_TRY_KILL} ]; then
                kill -KILL ${pid}
                tried_kill+=1
            else
                cur_phase+=1
            fi
        else
            break
        fi
        sleep 0.6
    done
    if zis_running ${pid}; then
        zdie "failed on kill process(${pid}) of app '${appname}', seems to be a defunct!"
    fi
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
    if [ ! -d "${approot}" ]; then
        mkdir -p ${approot}
        chown -R ${username}:${usergroup} ${approot}
    fi
    ${app_install_command} || zdie "failed on installing app '${appname}', command: ${app_install_command}"
    chown -R ${username}:${usergroup} ${approot} || zdie "failed on chown for app '${appname}'"
    zmkchroot ${appchroot} && return ${Z_SUCCESS} || return ${Z_FAIL}
}

zdelete_app() {
    local directory
    if [ -n "${appchroot}" ]; then
        cd /
        for directory in "${MOUNT_DIRECTORIES[@]}";
        do
            if ! umount ${appchroot}${directory}; then
                zdie "failed on 'umount ${appchroot}${directory}'"
            else
                sleep 0.1
                if zis_dir_empty "${appchroot}${directory}"; then
                    rm -fr ${appchroot}${directory}
                fi
            fi
            sleep 0.2
        done
        rm -fr ${appchroot}/src ${appchroot}/dev
        if zis_dir_empty "${appchroot}"; then
            rm -fr ${appchroot}
        else
            zdie "fatal error: some directories still stay inside '${appchroot}' after app removed."
        fi
    elif [ -n "${approot}" ]; then
        rm -fr ${approot}
    fi
    if ! zdelete_user ${username}; then
        zpush_error "failed on deleting user '${username}'"
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
    zapp_exists || zdie "app '${appname}' non-exists."
    zfinish || zdie "failed on stopping '${appname}'"
    zdelete_app || zdie "failed on remove app '${appname}'"
    return ${Z_SUCCESS}
}

zstart() {
    zis_running && return ${Z_SUCCESS}      # bug...
    zinstall || zdie
    zrun || zdie
    zadjust || zdie
    if [ -S ${ROUTER_SOCK} ]; then
        declare -i pid=$(zgetpid)
        local port="$(zget_listening ${pid})"
        echo -e "on$(echo ${act} | sed 's/^[a-z]/\U&/')\n${appname}\n${port}\n${pid}" | ${COMMAND_NC} -U ${ROUTER_SOCK}
    fi
    return ${Z_SUCCESS}
}

zrestart() {
    zstop && zstart || return ${Z_FAIL}
    return ${Z_SUCCESS}
}

zremove() {
    zapp_exists || zdie "app '${appname}' non-exists."
    zfinish || zdie "failed on stopping '${appname}'"
    zdelete_app || zdie "failed on remove app '${appname}'"
    return ${Z_SUCCESS}
}

zmkchroot() {
    test -n "${1}" || return ${Z_FAIL}
    local directory
    for directory in "${MOUNT_DIRECTORIES[@]}";
    do
        local target="${1}${directory}"
        if [ ! -d  "${target}" ]; then
            mkdir -p "${target}"
        fi
        if [ "${directory:0:4}" == "/var" -o "${directory:0:5}" == "/proc" ]; then
            mount --bind "${directory}" "${target}" 2>/dev/null || zdie "failed on mounting --bind '${directory}' '${target}'"
        else
            mount --bind "${directory}" "${target}" 2>/dev/null || zdie "failed on mounting --bind '${directory}' '${target}'"
        fi
    done
    local devdir="${1}/dev"
    if [ -n "${devdir}" -a ! -d  "${devdir}" ]; then
        mkdir -p ${devdir}
    fi
    test -e ${devdir}/null && \rm -f ${devdir}/null
    mknod ${devdir}/null c 1 3
    chmod a+rw ${devdir}/null
    return ${Z_SUCCESS}
}

zinitsys() {
    declare -a directories=("${APPS_ROOT}" "${APPS_LOG_DIR}" "${APP_USER_HOME_ROOT}" "${APPS_PIDFILE_DIR}")
    local directory
    for directory in "${directories[@]}";
    do
        if [ ! -d "${directory}" ]; then
            mkdir -p "${directory}" || zdie "failed on 'mkdir -p ${directory}'"
            chgrp ${usergroup} ${directory}
            chmod g+rw -R ${directory}
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
    return ${Z_SUCCESS}
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
    . ${zconfig}
    zcheck_depends "${DEPEND_UTILS[@]}" || zdie
    zcheck_config || zdie "wrong config"
}

usage () {
    echo "usage:"
    printf "    %s %s %s\n" "$(zcoblue $0)" "$(zcogreen '<appname>')" "$(zcogreen '[status|start|stop|restart|install|remove|checkname]')"
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

remove() {
    prepare
    if zremove; then
        zcogreen "${appname} removed"
        return ${Z_SUCCESS}
    else
        zcored "failed on removeing ${appname}"
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
    . ${zconfig}
    if zcheck_appname; then
        zcogreen "appname '${appname}' is available"
        return ${Z_SUCCESS}
    else
        zemit_errors
        return ${Z_FAIL}
    fi
}

initsys() {
    . ${zconfig}
    if zinitsys; then
        zcogreen "system successfully initialized"
        return ${Z_SUCCESS}
    else
        zcored "failed on initializing system"
        return ${Z_FAIL}
    fi
}

# entry of scripts
declare -r zconfig="$(dirname $(which $0))/zconfig.sh"
test -f "${zconfig}" || zdie "zconfig.sh ${zconfig} not found."

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
initsys) initsys;;
start)  start;;
stop)   stop;;
restart)    restart;;
install) install;;
'remove'|'uninstall') remove;;
checkname) checkname;;
'--help'|'-h') usage;;
*) unknown_act ${act};;
esac
