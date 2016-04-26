/*
 * file_md.c
 *
 *  Created on: Apr 23, 2016
 *      Author: sunil3590
 */

#include "constants.h"
#include "uthash.h"
#include "file_md.h"

int get_md_priority(file_md_t *file_md) {
	return file_md->mfu_ctr; // TODO : use LRU also
}
