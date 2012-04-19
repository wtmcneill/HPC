#!/bin/bash

cd /home/mcneillw/HPC/lab8/work

icc -openmp stream_kmp.c –o stream_kmp_affinity

export OMP_NUM_THREADS=6
export KMP_AFFINITY=verbose,compact
echo "compact:"
./stream_kmp
export KMP_AFFINITY=verbose,scatter
echo "scatter:"
./stream_kmp

