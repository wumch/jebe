#!/bin/sh

# global configs
APPS_ROOT=/apps
APP_USER_HOME_ROOT=/home
APP_USER_GROUP=apps
APPS_PIDFILE_DIR=/var/run/apps
APPS_LOG_DIR=/var/log
FIFO_NAME=/tmp/logger.pipe
COMMAND_INSTALL_APP="node install.js"
COMMAND_WATCH_FIFO="node logger.js"
declare -a RESERVED_APP_NAME=("system" "root" "initsys")
declare -a DEPEND_UTILS=("lsof" "netstat" "start-stop-daemon" "iptables" "node")

# limits
DEFAULT_ULIMITS="-u 1 -n 512 -v 131072 -m 131072 -i 1024 -f 0 -t 10 -q 128000 -x 128"
DEFAULT_NICE_INCR="19"

# variable
#appname="testingapp"
portalfile="index.js"

# auto-config
usergroup="${APP_USER_GROUP}"
username="${appname}"
approot="${APPS_ROOT}/${appname}"
appchroot="${approot}"
userhome="${APP_USER_HOME_ROOT}/${username}"
ulimits="${DEFAULT_ULIMITS}"
niceincr="${DEFAULT_NICE_INCR}"
pidfile=${APPS_PIDFILE_DIR}/${appname}.pid
app_install_command="${COMMAND_INSTALL_APP} ${appname}"
appstdout=${APPS_LOG_DIR}/${appname}.log
appstderr=${APPS_LOG_DIR}/${appname}.error.log

# test only
COMMAND_INSTALL_APP="ls"
COMMAND_WATCH_FIFO="ls"
DEFAULT_ULIMITS=""
DEFAULT_NICES=""
