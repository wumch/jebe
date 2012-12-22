#!/bin/sh

# global variables (not options).
exename="svd"

# PETSc options:
svd_nsv=""
svd_ncv=""
mat_type="mpiaij"

# our own options:
config="etc/${exename}.conf"

root_path=$(dirname $(realpath $0))
root_path="${root_path%/bin}"

cd ${root_path}
exepath="${root_path}/${exename}"

if [ ! -x "${exepath}" ]; then
    printf "\0[[${exepath}] is not a execute-able.\0[\033m\n"
    exit 1
fi

run() {
    ${exepath}
}

run

