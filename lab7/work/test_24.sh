#!/bin/bash

cd /home/mcneillw/HPC/lab7/work

. /curc/tools/utils/dkinit #so we can use the "reuse" command
reuse ICS #otherwise cant find some omp shared object

make

export OMP_NUM_THREADS=24
./omp_MatVecMultiply -m 80000 -n 8000
