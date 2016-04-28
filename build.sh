#!/bin/bash

(cd ./lib/pqueue && exec sh build.sh)
(cd ./build && exec make clean all)
mv ./build/sshdd ./sshdd