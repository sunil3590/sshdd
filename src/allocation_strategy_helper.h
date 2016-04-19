#ifndef ALLOCATION_STRATEGY_HELPER_H_
#define ALLOCATION_STRATEGY_HELPER_H_

typedef struct as_node_t
{
	file_md_t *file_md_ptr;
	size_t pos;
} as_node_t;


int cmp_pri(pqueue_pri_t next, pqueue_pri_t curr);

int cmp_pri_max(pqueue_pri_t next, pqueue_pri_t curr);

int cmp_pri_min(pqueue_pri_t next, pqueue_pri_t curr);

pqueue_pri_t get_pri(void *a);

void set_pri(void *a, pqueue_pri_t pri);

size_t get_pos(void *a);

void set_pos(void *a, size_t pos);

void printq_elem(FILE *out, void *elem);

int get_folder_size(char *folder);

int get_file_size(char *fname);

#endif /* ALLOCATION_STRATEGY_HELPER_H_ */
