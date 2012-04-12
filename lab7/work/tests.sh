#!/bin/bash

cd /home/mcneillw/HPC/lab7/work

rm -f results.txt
touch results.txt

for i in 2 4 8 12; do
	export OMP_NUM_THREADS=$i
	./omp_MatVecMultiply -m 80000 -n 8000 >> results.txt
done
