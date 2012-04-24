#!/bin/bash

cd /home/mcneillw/HPC/lab8/

./compile.sh

for i in 100 200 400 800 1600 3200; do
	./jacobiSS $i
done
