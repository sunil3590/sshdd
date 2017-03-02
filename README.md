# SSHDD
#### (ssshh.. we do it silently in the background)
#### Using SSD and HDD in a hybrid storage system
Files are moved between SSD and HDD without the user's knowledge with a goal to optimize file IO performance. The file movement is decided based on the access pattern and is dynamic in nature.

### Algorithm<hr>
* Maps file ID to physical file path
* SSD is always kept full
* Keep hot files on SSD
* Makes use of least recently used and most frequently used
* File swaps between SSD and HDD are made only when the priority differ by a threshold

### Testing<hr>
File access pattern data set : http://ita.ee.lbl.gov/html/contrib/WorldCup.html. All files in the access pattern data set are generated and the access pattern is simulated to study the performance gain in terms of time take to read all files in the access pattern.

### Dependencies<hr>
1. libpqueue
```
git clone cd https://github.com/vy/libpqueue.git
cd libpqueue/src
gcc -c pqueue.c -o pqueue.o
ar rcs libpqueue.a pqueue.o
```

### To run demo<hr>
#### Requirements
1. Ubuntu 14.04 machine
2. USB pendrive

#### Important
To increase the number of messages that can be held in message queue
>`sudo sh -c 'echo 8000 > /proc/sys/fs/mqueue/msg_max'`

#### Build
>`sh build.sh`

#### Pendrive setup and config
1. Create a folder in pendrive, name of your choice
2. Open run.sh and change path of USB folder

#### Run
>`sh run.sh`

This will run the 4 test cases as illustarted in the paper / presentation
Time taken for each of the 4 tests can be verified from console prints

### Results<hr>
Increased the read throughput by approximately 100% when compared to a static allocation strategy

### TODO<hr>
* Add locking mechanism while modifying metadata
