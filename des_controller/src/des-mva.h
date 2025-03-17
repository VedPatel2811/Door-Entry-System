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
	IDLE_MSG = 0,
	LEFT_SCAN_MSG = 1,
	GUARD_LEFT_UNLOCK_MSG = 2,
	LEFT_OPEN_MSG = 3,
	WEIGHT_UPDATE_MSG = 4,
	LEFT_CLOSE_MSG = 5,
	GUARD_LEFT_LOCK_MSG = 6,
	GUARD_RIGHT_UNLOCK_MSG = 7,
	RIGHT_OPEN_MSG = 8,
	RIGHT_CLOSE_MSG = 9,
	GUARD_RIGHT_LOCK_MSG = 10,
	RIGHT_SCAN_MSG = 11,
	EXIT_MSG = 12,
} Output;

const char *outMessage[NUM_OUTPUTS] = {
		"System is idle.",
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

#define TOTAL_INPUTS 12
const char *input_commands[TOTAL_INPUTS] = {
	"ls",  // Left scan
	"rs",  // Right scan
	"ws",  // Weight scale
	"lo",  // Left open
	"ro",  // Right open
	"lc",  // Left close
	"rc",  // Right close
	"grl", // Guard lock right
	"gru", // Guard unlock right
	"gll", // Guard lock left
	"glu", // Guard unlock left
	"Exit" // Terminate command
};

typedef struct {
	Output message_type;
	int person_id;
	int weight;
	int current_state;
} Person;

#endif /* DES_MVA_H_ */
