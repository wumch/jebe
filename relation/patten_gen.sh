#!/bin/bash

cd $(dirname $(realpath $0))

wordfile="{$1-../cws/etc/words.txt}"
outfile="${2-etc/patten.txt}"

awk '{if (NR>1){print $1}}' ${wordfile} > ${outfile}

