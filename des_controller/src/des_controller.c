#include <stdio.h>
#include <stdlib.h>
#include "des-mva.h"

void* handleLeftScan(void);
void* handleGuardLeftUnlock(void);
void* handleLeftOpen(void);
void* handleWeightScan(void);
void* handleLeftClose(void);
void* handleGuardLeftLock(void);
void* handleGuardRightUnlock(void);
void* handleRightOpen(void);
void* handleRightClose(void);
void* handleGuardRightLock(void);
void* handleRightScan(void);
void* handleExit(void);

int main(void) {
	Data msg;
	int currentState = LEFT_SCAN;
	int rcvid;
	int child = ChannelCreate(0);
	if (child == -1) {
		perror("Channel Create failure");
		return EXIT_FAILURE;
	}
	printf("Controller PID : %d\n", getpid());

	while (1) {
		rcvid = MsgReceive(child, &msg, sizeof(msg), NULL);
		if (rcvid == -1) {
			perror("MsgReceive failed");
			continue;
		}

		switch(currentState){
		case LEFT_SCAN:
			currentState = (int)handleLeftScan();
			break;
		case GUARD_LEFT_UNLOCK:
			currentState = (int)handleGuardLeftUnlock();
			break;
		case
		}

	}
}
