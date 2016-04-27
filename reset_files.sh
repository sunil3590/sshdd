INT=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/disk
INT_FILES=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/disk/*
EXT=/media/sunil3590/D017-4C6C
EXT_FILES=//media/sunil3590/D017-4C6C/*
BANK=/media/windows/Users/Sunil/Everything/MS/Courses/CSC568_ESA/Project/data/bank/.

rm -rf $EXT_FILES
cp -R $BANK $EXT

rm -rf $INT_FILES
cp -R $BANK $INT