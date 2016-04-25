#!/bin/bash

sudo sh -c 'echo 8000 > /proc/sys/fs/mqueue/msg_max'

SSD_FOLDER=./test/output/ssd
HDD_FOLDER=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/passport_sshdd/hdd

echo "Generating files ..."
#./Debug/sshdd generate $SSD_FOLDER $HDD_FOLDER > generate.log
echo "Complete\n"

echo "Simple test ..."
#./Debug/sshdd simple $SSD_FOLDER $HDD_FOLDER > simple.log
echo "Complete\n"

echo "Benchmarking ..."
#./Debug/sshdd benchmark $SSD_FOLDER $HDD_FOLDER 0 > benchmark.log
echo "Complete\n"

echo "Profiling SSHDD ..."
./Debug/sshdd profile $SSD_FOLDER $HDD_FOLDER 0 > profile.log
echo "Complete"