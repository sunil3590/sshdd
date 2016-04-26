#!/bin/bash

sudo sh -c 'echo 8000 > /proc/sys/fs/mqueue/msg_max'

SMALL_INT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/code/sshdd/test/output/ssd
BIG_INT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/code/sshdd/test/output/hdd
BIG_EXT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/passport_sshdd/hdd
SMALL_EXT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/passport_sshdd/ssd
SMALL_BANK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/files/ssd
BIG_BANK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/files/hdd

echo "Generating files ..."
./Debug/sshdd generate $SMALL_BANK $BIG_BANK > generate.log
echo "Complete"

echo "Simple test ..."
./Debug/sshdd simple $SMALL_BANK $BIG_BANK > simple.log
echo "Complete"

echo "Copying files to internal and external disks ..."
sh reset_files.sh
echo "Complete"

echo "Benchmarking internal disk ..."
./Debug/sshdd benchmark $SMALL_INT $BIG_INT 0 > benchmark_int.log
echo "Complete"

echo "Benchmarking external disk ..."
./Debug/sshdd benchmark $SMALL_EXT $BIG_EXT 0 > benchmark_ext.log
echo "Complete"

echo "Benchmarking hybrid ..."
./Debug/sshdd benchmark $SMALL_INT $BIG_EXT 0 > benchmark_ext.log
echo "Complete"

echo "Profiling SSHDD ..."
./Debug/sshdd profile $SMALL_INT $BIG_EXT 0 > profile.log
echo "Complete"