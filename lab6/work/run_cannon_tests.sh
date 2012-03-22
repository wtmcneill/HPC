#!/bin/bash

cd /home/mcneillw/HPC/lab6/work
rm results_c/time_size.txt
rm results_c/time_procs.txt

echo Working in $(pwd)

make fox
make canon

#amdahls
for i in 1024 2048 4096; do
	mpiexec -np 64 ./canons_multiply -size $i -verbose 1>> results_c/serial.txt 2>> results_c/err.log
done

echo finished serial, starting parallel

for i in 1 4 16 64; do
	for j in 1024 2048 4096 8192; do
		mpiexec -np $i ./canons_multiply -size $j -verbose  1>> results_c/parallel.txt 2>> results_c/err.log
	done
done
