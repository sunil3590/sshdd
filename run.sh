#!/bin/bash

#echo "Generating files"
#echo "---------------\n"
#./Debug/sshdd generate ./test/output/ssd '/media/sunil3590/My Passport/sshdd/hdd' > generate.log

echo "\n\nSimple test"
echo "---------------\n"
./Debug/sshdd simple ./test/output/ssd '/media/sunil3590/My Passport/sshdd/hdd' > simple.log

echo "\n\nBenchmarking"
echo "---------------\n"
./Debug/sshdd benchmark ./test/output/ssd '/media/sunil3590/My Passport/sshdd/hdd' 0 > benchmark.log

echo "\n\nProfiling SSHDD"
echo "---------------\n"
./Debug/sshdd profile ./test/output/ssd '/media/sunil3590/My Passport/sshdd/hdd' 0 > profile.log