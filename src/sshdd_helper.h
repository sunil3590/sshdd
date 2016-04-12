/*
 * sshdd_helper.h
 *
 *  Created on: 03-Apr-2016
 *      Author: sunil3590
 */

#ifndef SSHDD_HELPER_H_
#define SSHDD_HELPER_H_

int build_metadata_for_folder(const char *folder, file_loc loc,
		file_md_t *file_md, file_md_t **ht_head);

#endif /* SSHDD_HELPER_H_ */
