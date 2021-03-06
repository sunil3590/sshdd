/*
 * file_md.h
 *
 *  Created on: 03-Apr-2016
 *      Author: sunil3590
 */

#ifndef FILE_MD_H_
#define FILE_MD_H_

// enum to specify the location of a file
typedef enum file_loc {
	HDD,
	SSD,
} file_loc;

// File metadata strcuture
typedef struct file_md_t {
	char fileid[FNAME_SMALL_SIZE];	// KEY
	file_loc loc;					// hdd or ssd
    int mfu_ctr;					// MFU counter
    // int lru_ctr;					// LRU counter TODO
    int ref_count;					// marked if file is open
    UT_hash_handle hh;				// makes this structure hashable
} file_md_t;

int get_md_priority(file_md_t *file_md);

#endif /* FILE_MD_H_ */
