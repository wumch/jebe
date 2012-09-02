#!/bin/sh

getppid() {
    local program="${1}"
    printf "%s\t" $program
    if [ -n "${program}" ]; then
        for pid in $(pidof ${program});
        do
            declare -i ppid="$(awk -F': *' '/PPid:/ {print $2}' /proc/${pid}/status 2>/dev/null)"
            if [ -n "${ppid}" ] && [ $((ppid>1)) ]; then
                echo ${ppid}
                return 0
            fi
        done
    fi
    return 1
}

newerthan() {
    echo ""
}

getppid nginx

