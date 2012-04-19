#!/bin/bash

. /curc/tools/utils/dkinit #so we can use the "reuse" command
reuse .openmpi-1.4.3_gcc-4.5.2_torque-2.5.8_ib
mpicxx -O2 -fopenmp trap.cpp -o trap

mpiexec -np 1 ./trap
