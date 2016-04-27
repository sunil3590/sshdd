#!/bin/bash

sudo sh -c 'echo 8000 > /proc/sys/fs/mqueue/msg_max'

SMALL_DISK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/disk/ssd
BIG_DISK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/disk/hdd
SMALL_USB=/media/sunil3590/D017-4C6C/ssd
BIG_USB=/media/sunil3590/D017-4C6C/hdd
SMALL_BANK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/bank/ssd
BIG_BANK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/bank/hdd

echo "Generating files ..."
./Debug/sshdd generate $SMALL_BANK $BIG_BANK > generate.log
echo "Complete\n"

echo "Copying files to internal and external disks ..."
sh reset_files.sh
echo "Complete\n"

echo "Benchmarking hard disk - free run ..."
./Debug/sshdd benchmark $SMALL_DISK $BIG_DISK 0 > benchmark_disk.log
echo "Complete\n"

echo "Benchmarking USB pendrive - free run ..."
./Debug/sshdd benchmark $SMALL_USB $BIG_USB 0 > benchmark_usb.log
echo "Complete\n"

echo "Benchmarking hybrid - free run..."
./Debug/sshdd benchmark $SMALL_USB $BIG_DISK 0 > benchmark_free.log
echo "Complete\n"

echo "Profiling SSHDD - free run ..."
./Debug/sshdd profile $SMALL_USB $BIG_DISK 0 > profile_free.log
echo "Complete\n"

echo "Copying files to internal and external disks ..."
sh reset_files.sh
echo "Complete\n"

echo "Benchmarking hybrid - timed..."
./Debug/sshdd benchmark $SMALL_USB $BIG_DISK 1 > benchmark_realtime.log
echo "Complete\n"

echo "Profiling SSHDD - timed ..."
./Debug/sshdd profile $SMALL_USB $BIG_DISK 1 > profile_realtime.log
echo "Complete"