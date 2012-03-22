#!/bin/bash

cd /home/mcneillw/labs/lab6/work
rm results_f/time_size.txt
rm results_f/time_procs.txt

echo Working in $(pwd)

#amdahls
for i in 1024 2048 4096
do
	mpiexec -np 64 ./fox_multiply -size $i -verbose 1>> results_f/serial.txt 2>> results_f/err.log
done

echo finished serial, starting parallel

for i in 1 4 16 64
do
	for j in 1024 2048 4096 8192
	do
		mpiexec -np $i ./fox_multiply -size $j -verbose  1>> results_f/parallel.txt 2>> results_f/err.log
	done
done
