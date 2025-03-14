#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include "../../des_controller/src/des-mva.h"


int main(void) {
	Person msg;
	int rcvid;

	//Phase I
	int child = ChannelCreate(0);
	if (child == -1) {
		perror("Channel Create failure");
		return EXIT_FAILURE;
	}

	printf("PID : %d\n", getpid());

	//Phase II
	while (1) {
		rcvid = MsgReceive(child, &msg, sizeof(msg), NULL);
		if (rcvid == -1) {
			perror("MsgReceive failed");
			continue;
		}

		MsgReply(rcvid, EOK, NULL, 0);

		if (msg.message_type == LEFT_SCAN_MSG  || msg.message_type ==  RIGHT_SCAN_MSG ) {
			printf("%s %d\n", outMessage[msg.message_type - 1], msg.person_id);
		} else if (msg.message_type == WEIGHT_UPDATE_MSG ) {
			printf("%s %d kg\n", outMessage[msg.message_type - 1], msg.weight);
		} else if (msg.message_type == EXIT_MSG ) {
			printf("%s\n", outMessage[msg.message_type - 1]);
			break;
		} else {
			printf("%s\n", outMessage[msg.message_type - 1]);
		}
	}

	//Phase III
	int destroyChild = ChannelDestroy(child);
	if (destroyChild == -1) {
		perror("Destroy Child failure");
		return EXIT_FAILURE;
	}
	return EXIT_SUCCESS;
}
