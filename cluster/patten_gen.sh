#!/bin/bash

ROOT_PATH=$(dirname $(realpath $0))
ROOT_PATH=${ROOT_PATH%/bin}
cd "${ROOT_PATH}"

wordfile="${1:-../cws/etc/words.txt}"
outfile="${2:-etc/patten.txt}"

awk '{if (NR>1){print $1}}' ${wordfile} > ${outfile}

