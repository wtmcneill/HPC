#!/bin/bash

cd /home/mcneillw/HPC/lab8/work

. /curc/tools/utils/dkinit #so we can use the "reuse" command
reuse ICS

icc -openmp stream_kmp.c -o stream_kmp_affinity

export OMP_NUM_THREADS=6
export KMP_AFFINITY=verbose,compact
echo "compact:"
./stream_kmp
export KMP_AFFINITY=verbose,scatter
echo "scatter:"
./stream_kmp

