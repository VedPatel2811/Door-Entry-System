#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>
#include "../../des_controller/src/des-mva.h"

int get_command_index(const char *input) {
	for (int i = 0; i < TOTAL_INPUTS; i++) {
		if (strcmp(input, input_commands[i]) == 0) {
			return i;
		}
	}
	return -1; // Invalid command
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <controller_pid>\n", argv[0]);
		return EXIT_FAILURE;
	}

	pid_t controller_pid = atoi(argv[1]);

	// Attach to the controller's channel
	int coid = ConnectAttach(ND_LOCAL_NODE, controller_pid, 1,
	_NTO_SIDE_CHANNEL, 0);
	if (coid == -1) {
		perror("ConnectAttach failed");
		return EXIT_FAILURE;
	}

	while (1) {
		printf("\nSelect event from the following list:\n");
		printf(" ls  - Left Scan\n");
		printf(" rs  - Right Scan\n");
		printf(" ws  - Weight Scale\n");
		printf(" lo  - Left Open\n");
		printf(" ro  - Right Open\n");
		printf(" lc  - Left Close\n");
		printf(" rc  - Right Close\n");
		printf(" grl - Guard Right Lock\n");
		printf(" gru - Guard Right Unlock\n");
		printf(" gll - Guard Left Lock\n");
		printf(" glu - Guard Left Unlock\n");
		printf(" Exit - Exit the program\n");
		printf("Enter choice: ");

		char user_input[20];
		Person person_message;

		scanf("%19s", user_input);

		int cmd_index = get_command_index(user_input);

		switch (cmd_index) {
		case 0: // "ls"
			person_message.current_state = LEFT_SCAN;
			printf("Enter the Person's ID: ");
			fflush(stdout);
			scanf("%d", &person_message.person_id);
			printf("\n");
			break;
		case 1: // "rs"
			person_message.current_state = RIGHT_SCAN;
			printf("Enter the Person's ID: ");
			fflush(stdout);
			scanf("%d", &person_message.person_id);
			printf("\n");
			break;
		case 2: // "ws"
			person_message.current_state = WEIGHT_SCAN;
			printf("Enter the Person's Weight: ");
			fflush(stdout);
			scanf("%d", &person_message.weight);
			printf("\n");
			break;
		case 3: // "lo"
			person_message.current_state = LEFT_OPEN;
			break;
		case 4: // "ro"
			person_message.current_state = RIGHT_OPEN;
			break;
		case 5: // "lc"
			person_message.current_state = LEFT_CLOSE;
			break;
		case 6: // "rc"
			person_message.current_state = RIGHT_CLOSE;
			break;
		case 7: // "grl"
			person_message.current_state = GUARD_RIGHT_LOCK;
			break;
		case 8: // "gru"
			person_message.current_state = GUARD_RIGHT_UNLOCK;
			break;
		case 9: // "gll"
			person_message.current_state = GUARD_LEFT_LOCK;
			break;
		case 10: // "glu"
			person_message.current_state = GUARD_LEFT_UNLOCK;
			break;
		case 11: // "terminate"
			person_message.current_state = EXIT;
			break; // Exit the program
		default:
			printf("Invalid command. Please try again.\n");
		}

		// Send the Person object to the controller
		if (MsgSend(coid, &person_message, sizeof(person_message), NULL, 0) == -1) {
			perror("MsgSend failed");
			ConnectDetach(coid);
			exit(EXIT_FAILURE);
		}
		if(person_message.current_state == EXIT){
			break;
		}

	}

	// Detach from the controller's channel
	ConnectDetach(coid);
	printf("Disconnected from controller. Exiting...\n");

	return EXIT_SUCCESS;
}
