# SSHDD
#### (ssshh.. we do it silently in the background)
#### Using SSD as a fast cache in a tiered architecture along with HDD
Files are moved between SSD and HDD without the user's knowledge with a goal to optimize file IO performance. The file movement is decided based on the access pattern and dynamic in nature.

### Algorithm
* Maps file ID to physical file path
* SSD is always kept full
* Makes use of least recently used and most frequently used
* File swaps between SSD and HDD are made only when the scores differ by a threshold

### Testing
File access pattern data set : http://ita.ee.lbl.gov/html/contrib/WorldCup.html. All files in the access pattern data set are generated and the access pattern is simulated to study the performance gain in terms of time take to read all files in the access pattern.

### TODO
* Ensure the profiler does not read files out of RAM from the second time a file is accessed
* Add mapping of file ID to physical path in SSHDD
* Implement file access book keeping in SSHDD
* Implement file allocation strategy
* Remove hard coding of numbers
* Add locking mechanism

### Dependencies
1. libpqueue
```
git clone cd https://github.com/vy/libpqueue.git
cd libpqueue/src
gcc -c pqueue.c -o pqueue.o
ar rcs libpqueue.a pqueue.o
```