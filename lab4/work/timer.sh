#!/bin/bash

cd /home/mcneillw/labs/lab4/work
rm results/time_size.txt
rm results/time_procs.txt

reuse ICS
reuse OpenMPI-1.4-ICS

echo Working in $(pwd)

for i in 2 4 8 16 32 64 128
do
	mpiexec -np $i ./allreduce_timer 1 >> results/time_procs.txt
done

echo Finished process sizes, starting message size

for i in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096 8192 16384 32768 65536 131072
do
	mpiexec -np 128 ./allreduce_timer $i  >> results/time_size.txt
done
