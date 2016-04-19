/*
 * profile.h
 *
 *  Created on: 03-Mar-2016
 *      Author: sunil3590
 */

#ifndef PROFILE_H_
#define PROFILE_H_

typedef enum profile_what {
	DEFAULT_FUNCTIONS,
	SSHDD_FUNCTIONS
} profile_what;

int profile(profile_what what, int real_time);

#endif /* PROFILE_H_ */
