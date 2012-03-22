#!/bin/bash

cd /home/mcneillw/labs/lab6/work
rm results/time_size.txt
rm results/time_procs.txt

echo Working in $(pwd)

#amdahls
for i in 1024 2048 4096
do
	mpiexec -np 64 ./fox_multiply -size $i -verbose >> results/serial.txt
done

echo finished serial, starting parallel

for i in 1 4 16 64
do
	for j in 1024 2048 4096 8192
	do
		mpiexec -np $i ./fox_multiply -size $j -verbose  >> results/parallel.txt
	done
done
