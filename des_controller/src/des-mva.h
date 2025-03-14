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

#define NUM_OUTPUTS 12
typedef enum {
    LEFT_SCAN = 1, 				//ls
	READ_INPUT = 2,
    GUARD_LEFT_UNLOCK = 3,		//glu
	LEFT_OPEN = 4,				//lo
	WEIGHT_SCAN = 5,			//ws
	WEIGHT_UPDATE = 6,
	LEFT_CLOSE = 7,				//lc
	GUARD_LEFT_LOCK = 8,		//gll
	GUARD_RIGHT_UNLOCK = 9,		//gru
	RIGHT_OPEN = 10,				//ro
	RIGHT_CLOSE = 11,			//rc
	GUARD_RIGHT_LOCK = 12,		//grl
	RIGHT_SCAN = 13,			//rs
	EXIT = 14					//exit
} State;

typedef enum {

} Output;

const char *outMessage[NUM_OUTPUTS] = {
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
	"Exit."};										//exit

typedef struct{
	State message_type;
	int person_id;
	int weight;
} Input;

#endif /* DES_MVA_H_ */
