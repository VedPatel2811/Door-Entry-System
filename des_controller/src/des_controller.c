#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/neutrino.h>
#include <sys/dispatch.h>
#include <sys/netmgr.h>
#include <string.h>
#include <stdbool.h>
#include "des-mva.h"

static State current_state = IDLE;
static int display_coid;
bool returning = false;
bool finalExit = false;

State handle_idle(Person *event);
State handle_guard_left_unlock(Person *event);
State handle_left_open(Person *event);
State handle_weight_scan(Person *event);
State handle_left_close(Person *event);
State handle_guard_left_lock(Person *event);
State handle_guard_right_unlock(Person *event);
State handle_right_open(Person *event);
State handle_right_close(Person *event);
State handle_guard_right_lock(Person *event);
State handle_exit(Person *event);

void send_display_update(Output msg_type, int data) {
	Person display_msg = { .message_type = msg_type, .person_id = data,
			.weight = data };

	if (MsgSend(display_coid, &display_msg, sizeof(display_msg), NULL, 0)
			== -1) {
		perror("MsgSend to display failed");
	}
}

State handle_idle(Person *event) {
	switch (event->current_state) {
	case LEFT_SCAN:
		returning = false;
		send_display_update(LEFT_SCAN_MSG, event->person_id);
				return GUARD_LEFT_UNLOCK;

	case RIGHT_SCAN:
		returning = true;
		send_display_update(RIGHT_SCAN_MSG, event->person_id);
				return GUARD_RIGHT_UNLOCK;

	default:
		return IDLE;
	}
}

State handle_guard_left_unlock(Person *event) {
	if (event->current_state == GUARD_LEFT_UNLOCK) {
		send_display_update(GUARD_LEFT_UNLOCK_MSG, 0);
		return LEFT_OPEN;
	}
	return GUARD_LEFT_UNLOCK;
}

State handle_left_open(Person *event) {
	if (event->current_state == LEFT_OPEN && returning == false) {
		send_display_update(LEFT_OPEN_MSG, 0);
		return WEIGHT_SCAN;
	} else if (event->current_state == LEFT_OPEN && returning == true) {
		send_display_update(LEFT_OPEN_MSG, 0);
		return LEFT_CLOSE;
	}
	return LEFT_OPEN;
}

State handle_weight_scan(Person *event) {
	if (event->current_state == WEIGHT_SCAN && returning == false) {
		send_display_update(WEIGHT_UPDATE_MSG, event->weight);
		return LEFT_CLOSE;
	} else if (event->current_state == WEIGHT_SCAN && returning == true) {
		send_display_update(WEIGHT_UPDATE_MSG, event->weight);
		return RIGHT_CLOSE;
	}
	return WEIGHT_SCAN;
}

State handle_left_close(Person *event) {
	if (event->current_state == LEFT_CLOSE) {
		send_display_update(LEFT_CLOSE_MSG, 0);
		return GUARD_LEFT_LOCK;
	}
	return LEFT_CLOSE;
}

State handle_guard_left_lock(Person *event) {
	if (event->current_state == GUARD_LEFT_LOCK && returning == false) {
		send_display_update(GUARD_LEFT_LOCK_MSG, 0);
		return GUARD_RIGHT_UNLOCK;
	} else if (event->current_state == GUARD_LEFT_LOCK && returning == true) {
		send_display_update(GUARD_LEFT_LOCK_MSG, 0);
		return EXIT;
	}
	return GUARD_LEFT_LOCK;
}

State handle_guard_right_unlock(Person *event) {
	if (event->current_state == GUARD_RIGHT_UNLOCK) {
		send_display_update(GUARD_RIGHT_UNLOCK_MSG, 0);
		return RIGHT_OPEN;
	}
	return GUARD_RIGHT_UNLOCK;
}
State handle_right_open(Person *event) {
	if (event->current_state == RIGHT_OPEN && returning == false) {
		send_display_update(RIGHT_OPEN_MSG, 0);
		return RIGHT_CLOSE;
	} else if (event->current_state == RIGHT_OPEN && returning == true) {
		send_display_update(RIGHT_OPEN_MSG, 0);
		return WEIGHT_SCAN;
	}
	return RIGHT_OPEN;
}
State handle_right_close(Person *event) {
	if (event->current_state == RIGHT_CLOSE) {
		send_display_update(RIGHT_CLOSE_MSG, 0);
		return GUARD_RIGHT_LOCK;
	}
	return RIGHT_CLOSE;
}
State handle_guard_right_lock(Person *event) {
	if (event->current_state == GUARD_RIGHT_LOCK && returning == false) {
		send_display_update(GUARD_RIGHT_LOCK_MSG, 0);
		return EXIT;
	} else if (event->current_state == GUARD_RIGHT_LOCK && returning == true) {
		send_display_update(GUARD_RIGHT_LOCK_MSG, 0);
		return GUARD_LEFT_UNLOCK;
	}
	return GUARD_RIGHT_LOCK;
}
State handle_exit(Person *event) {
	if (event->current_state == EXIT) {
		send_display_update(EXIT_MSG, 0);
		finalExit = true;
		return EXIT;
	} else if (event->current_state == RIGHT_SCAN && returning == false) {
		returning = true;
		send_display_update(RIGHT_SCAN_MSG, event->person_id);
		return GUARD_RIGHT_UNLOCK;
	}
	return EXIT;
}

int main(int argc, char *argv[]) {
	if (argc != 2) {
		fprintf(stderr, "Usage: %s <display_pid>\n", argv[0]);
		return EXIT_FAILURE;
	}

	pid_t display_pid = atoi(argv[1]);

	int chid = ChannelCreate(0);
	if (chid == -1) {
		perror("ChannelCreate failed");
		return EXIT_FAILURE;
	}

	printf("Controller PID: %d\n", getpid());

	display_coid = ConnectAttach(ND_LOCAL_NODE, display_pid, 1,
	_NTO_SIDE_CHANNEL, 0);
	if (display_coid == -1) {
		perror("ConnectAttach to display failed");
		return EXIT_FAILURE;
	}

	Person event;
	int rcvid;

	while (1) {
		rcvid = MsgReceive(chid, &event, sizeof(event), NULL);

		switch (current_state) {
		case IDLE:
			current_state = handle_idle(&event);
			break;
		case GUARD_LEFT_UNLOCK:
			current_state = handle_guard_left_unlock(&event);
			break;
		case LEFT_OPEN:
			current_state = handle_left_open(&event);
			break;
		case WEIGHT_SCAN:
			current_state = handle_weight_scan(&event);
			break;
		case LEFT_CLOSE:
			current_state = handle_left_close(&event);
			break;
		case GUARD_LEFT_LOCK:
			current_state = handle_guard_left_lock(&event);
			break;
		case GUARD_RIGHT_UNLOCK:
			current_state = handle_guard_right_unlock(&event);
			break;
		case RIGHT_OPEN:
			current_state = handle_right_open(&event);
			break;
		case RIGHT_CLOSE:
			current_state = handle_right_close(&event);
			break;
		case GUARD_RIGHT_LOCK:
			current_state = handle_guard_right_lock(&event);
			break;
		case EXIT:
			current_state = handle_exit(&event);
			break;
		default:
			current_state = IDLE;
			break;
		}

		MsgReply(rcvid, EOK, NULL, 0);

		if (finalExit) {
			break;
		}
	}

	ConnectDetach(display_coid);
	ChannelDestroy(chid);
	return EXIT_SUCCESS;
}
