/*
 * des-mva.h
 *
 *  Created on: Mar. 13, 2025
 *      Author: User
 */

#ifndef DES_MVA_H_
#define DES_MVA_H_

#include <sys/neutrino.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

typedef enum {
	IDLE = 0,
	LEFT_SCAN = 1, 				//ls
	GUARD_LEFT_UNLOCK = 2,		//glu
	LEFT_OPEN = 3,				//lo
	WEIGHT_SCAN = 4,			//ws
	LEFT_CLOSE = 5,				//lc
	GUARD_LEFT_LOCK = 6,		//gll
	GUARD_RIGHT_UNLOCK = 7,		//gru
	RIGHT_OPEN = 8,				//ro
	RIGHT_CLOSE = 9,			//rc
	GUARD_RIGHT_LOCK = 10,		//grl
	RIGHT_SCAN = 11,			//rs
	EXIT = 12					//exit
} State;

#define NUM_OUTPUTS 13
typedef enum {
	IDLE_MSG = 1,
	LEFT_SCAN_MSG = 2,
	GUARD_LEFT_UNLOCK_MSG = 3,
	LEFT_OPEN_MSG = 4,
	WEIGHT_UPDATE_MSG = 5,
	LEFT_CLOSE_MSG = 6,
	GUARD_LEFT_LOCK_MSG = 7,
	GUARD_RIGHT_UNLOCK_MSG = 8,
	RIGHT_OPEN_MSG = 9,
	RIGHT_CLOSE_MSG = 10,
	GUARD_RIGHT_LOCK_MSG = 11,
	RIGHT_SCAN_MSG = 12,
	EXIT_MSG = 13,
} Output;

const char *outMessage[NUM_OUTPUTS] = {
		"System is idle."
		"Person scanned entering with ID: ",			//ls
		"Guard unlocked the left door.",				//glu
		"Left door is now open.",						//lo
		"Person has been weighed and their weight is ",	//ws
		"Left door is now closed.",						//lc
		"Guard locked the left door.",					//gll
		"Guard unlocked the right door.",				//gru
		"Right door is now open.",						//ro
		"Right door is now closed.",					//rc
		"Guard locked the right door.",					//grl
		"Person scanned leaving with ID: ",				//rs
		"Exit." };										//exit

typedef struct {
	Output message_type;
	int person_id;
	int weight;
	int current_state;
} Person;

typedef struct {
	Person individual;
	int status_code;
	char error_message[128];
} ControllerResponse;

#endif /* DES_MVA_H_ */
