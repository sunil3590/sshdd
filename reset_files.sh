SMALL_INT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/code/sshdd/test/output/ssd
BIG_INT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/code/sshdd/test/output/hdd
BIG_EXT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/passport_sshdd/hdd
SMALL_EXT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/passport_sshdd/ssd
SMALL_BANK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/files/ssd
BIG_BANK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/files/hdd

rm -rf $SMALL_EXT
mkdir $SMALL_EXT
cp $SMALL_BANK"/*" $SMALL_EXT

rm -rf $BIG_EXT
mkdir $BIG_EXT
cp $BIG_BANK"/*" $BIG_EXT

rm -rf $SMALL_INT
mkdir $SMALL_INT
cp $SMALL_BANK"/*" $SMALL_INT

rm -rf $BIG_INT
mkdir $BIG_INT
cp $BIG_BANK"/*" $BIG_INT