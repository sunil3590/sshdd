#!/bin/bash

git clone https://github.com/vy/libpqueue.git
cd ./libpqueue/src
gcc -c pqueue.c -o pqueue.o
ar rcs libpqueue.a pqueue.o
cp libpqueue.a ../../libpqueue.a
cp pqueue.h ../../pqueue.h