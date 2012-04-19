#!/bin/bash

cd /home/mcneillw/HPC/lab8/work

. /curc/tools/utils/dkinit #so we can use the "reuse" command
reuse ICS

icc -openmp stream_numa.c –o stream_numa

export OMP_NUM_THREADS=12
echo "regular stream_numa:"
./stream_numa

export OMP_NUM_THREADS=12
echo "interveaved stream_numa:"
numactl --interleave=all ./stream_numa
