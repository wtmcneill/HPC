#!/bin/bash

cd /home/mcneillw/HPC/lab6/work
rm results_f/serial.txt
rm results_f/parallel.txt

#reuse .openmpi-1.4.3_gcc-4.5.2_torque-2.5.8_ib

echo Working in $(pwd)

make fox
make canon

#amdahls
for i in 64 1024 2048 4096; do # the first is to "warm up" the timers - even when warming up in fox's, the first result was off
	mpiexec -np 64 ./fox_multiply -size $i -verbose 1>> results_f/serial.txt 2>> results_f/err.log
done

echo finished serial, starting parallel

for i in 1 4 16 64; do
	for j in 1024 2048 4096 8192; do
		mpiexec -np $i ./fox_multiply -size $j -verbose  1>> results_f/parallel.txt 2>> results_f/err.log
	done
done
