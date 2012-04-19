#!/bin/bash

cd /home/mcneillw/HPC/lab8/work

. /curc/tools/utils/dkinit #so we can use the "reuse" command
reuse .openmpi-1.4.3_gcc-4.5.2_torque-2.5.8_ib
mpicxx -O2 -fopenmp trap.cpp -o trap

export OMP_NUM_THREADS=12
mpiexec -np 1 ./trap
export OMP_NUM_THREADS=6
mpiexec -np 1 ./trap
mpiexec -np 4 ./trap
mpiexec -np 6 ./trap
