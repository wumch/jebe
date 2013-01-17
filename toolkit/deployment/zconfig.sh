#!/bin/sh

declare -i SSA_DEBUG=1

# global configs
APPS_ROOT=/apps
APP_USER_HOME_ROOT=/home
APP_USER_GROUP=apps
APPS_PIDFILE_DIR=/var/run/apps
APPS_LOG_DIR=/var/log/apps
FIFO_NAME=""
ROUTER_SOCK=/var/run/router.sock
COMMAND_WATCH_FIFO=""
COMMAND_NC=$(which nc.openbsd 2>/dev/null) || COMMAND_NC=${COMMAND_NC:-$(which nc 2>/dev/null)} || COMMAND_NC=${COMMAND_NC:-$(which netcat 2>/dev/null)}
COMMAND_INSTALL_APP="install.js"
COMMAND_LOGLINE="loger.js"
RESERVED_APP_NAME=("system" "root" "initsys")
DEPEND_UTILS=("netstat" "realpath" "node" "${COMMAND_NC}")
MOUNT_DIRECTORIES=("/usr" "/lib64" "/proc" "/var" "/bin" "/etc")
APPS_CHROOT="${APPS_ROOT}"

# limits
DEFAULT_ULIMITS="-u 1 -n 512 -v 131072 -m 131072 -i 1024 -f 0 -t 10 -q 128000 -x 128"
DEFAULT_NICE_INCR="19"

if [ -n "{$SSA_DEBUG}" -a ${SSA_DEBUG} -eq 1 ]; then
    # test only
    DEFAULT_ULIMITS="-n 1024"
    APPS_ROOT="/tmp/apps"
    APPS_CHROOT="${APPS_ROOT}"
fi

# this variable is guaranteed to be set inside start-stop-apps.
#appname="testingapp"

# auto-config
usergroup="${APP_USER_GROUP}"
username="${appname}"
appchroot="${APPS_CHROOT}/${appname}"
approot="${APPS_ROOT}/${appname}/src"
portalfile="index.js"
userhome="${APP_USER_HOME_ROOT}/${username}"
ulimits="${DEFAULT_ULIMITS}"
niceincr="${DEFAULT_NICE_INCR}"
pidfile=${APPS_PIDFILE_DIR}/${appname}.pid
app_install_command="${COMMAND_INSTALL_APP} ${appname} ${approot}"
appstdout=${APPS_LOG_DIR}/${appname}.log
appstderr=${APPS_LOG_DIR}/${appname}.error.log

declare -i uniqprocflag=$(date +'%s%N')
appprocflag="--max-stack-size=${uniqprocflag}"
assistprocflag="--max-stack-size=$((uniqprocflag+1))"
app_run_command="$(which node) ${appprocflag} ${portalfile} | ${COMMAND_LOGLINE} ${appname} ${appstdout} ${assistprocflag}"
