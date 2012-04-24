#!/bin/bash

cd /home/mcneillw/HPC/lab8/

. /curc/tools/utils/dkinit #so we can use the "reuse" command
reuse .pgi-12.3-testing

pgcc -ta=nvidia -Minfo -o jacobiSS jacobiSS.c
