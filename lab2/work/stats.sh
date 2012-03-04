#!/bin/bash

env > env2.txt

touch stats_rw.txt
touch stats_naive.txt
touch stats_block.txt
touch stats_intel.txt
touch stats_block_2.txt

./read_write_matrix m10.txt m10.hdf5 m10_out.txt >> stats_rw.txt
./read_write_matrix m100.txt m100.hdf5 m100_out.txt >> stats_rw.txt
./read_write_matrix m200.txt m200.hdf5 m200_out.txt >> stats_rw.txt
./read_write_matrix m500.txt m500.hdf5 m500_out.txt >> stats_rw.txt
./read_write_matrix m1000.txt m1000.hdf5 m1000_out.txt >> stats_rw.txt

./read_write_matrix m10.hdf5 m10.txt m10_out.txt >> stats_rw.txt
./read_write_matrix m100.hdf5 m100.txt m100_out.txt >> stats_rw.txt
./read_write_matrix m200.hdf5 m200.txt m200_out.txt >> stats_rw.txt
./read_write_matrix m500.hdf5 m500.txt m500_out.txt >> stats_rw.txt
./read_write_matrix m1000.hdf5 m1000.txt m1000_out.txt >> stats_rw.txt


./naive_multiply m10.txt m10.hdf5 m10_out.txt >> stats_naive.txt
./naive_multiply m100.txt m100.hdf5 m100_out.txt >> stats_naive.txt
./naive_multiply m200.txt m200.hdf5 m200_out.txt >> stats_naive.txt
./naive_multiply m500.txt m500.hdf5 m500_out.txt >> stats_naive.txt
./naive_multiply m1000.txt m1000.hdf5 m1000_out.txt >> stats_naive.txt

./block_multiply m10.txt m10.hdf5 m10_out.txt >> stats_block.txt
./block_multiply m100.txt m100.hdf5 m100_out.txt >> stats_block.txt
./block_multiply m200.txt m200.hdf5 m200_out.txt >> stats_block.txt
./block_multiply m500.txt m500.hdf5 m500_out.txt >> stats_block.txt
./block_multiply m1000.txt m1000.hdf5 m1000_out.txt >> stats_block.txt

./intel_multiply m10.txt m10.hdf5 m10_out.txt >> stats_intel.txt
./intel_multiply m100.txt m100.hdf5 m100_out.txt >> stats_intel.txt
./intel_multiply m200.txt m200.hdf5 m200_out.txt >> stats_intel.txt
./intel_multiply m500.txt m500.hdf5 m500_out.txt >> stats_intel.txt
./intel_multiply m1000.txt m1000.hdf5 m1000_out.txt >> stats_intel.txt

./block_multiply m1000.txt m1000.txt m10_out.txt 2 >> stats_block_2.txt
./block_multiply m1000.txt m1000.txt m100_out.txt 4 >> stats_block_2.txt
./block_multiply m1000.txt m1000.txt m200_out.txt 6 >> stats_block_2.txt
./block_multiply m1000.txt m1000.txt m500_out.txt 8 >> stats_block_2.txt
./block_multiply m1000.txt m1000.txt m1000_out.txt 10 >> stats_block_2.txt

