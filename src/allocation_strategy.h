#ifndef ALLOCATION_STRATEGY_H_
#define ALLOCATION_STRATEGY_H_

// TODO
#define THRESHOLD 5
#define SLEEP_DUR (1000 * 1000)

// Init function to start allocation strategy daemon
void * allocation_strategy(void *sshdd_handle);

#endif /* ALLOCATION_STRATEGY_H_ */
