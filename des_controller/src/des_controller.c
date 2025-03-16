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

bool exiting = false;
Person display_message;
int child, rcvid, coid;

State handle_idle();
State handle_left_scan();
State handle_guard_left_unlock();
State handle_left_open();
State handle_weight_scan();
State handle_left_close();
State handle_guard_left_lock();
State handle_guard_right_unlock();
State handle_right_open();
State handle_right_close();
State handle_guard_right_lock();
State handle_right_scan();
State handle_exit();

void send_to_display(Output msg) {
    display_message.message_type = msg;
    if (MsgSend(coid, &display_message, sizeof(display_message), NULL, 0) == -1) {
        perror("Message Send failure");
        ChannelDestroy(child);
        ConnectDetach(coid);
        exit(EXIT_FAILURE);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <display_pid>\n", argv[0]);
        return EXIT_FAILURE;
    }

    pid_t display_pid = atoi(argv[1]);

    child = ChannelCreate(0);
    if (child == -1) {
        perror("Channel Create failure");
        return EXIT_FAILURE;
    }

    printf("CalcServer PID : %d\n", getpid());

    coid = ConnectAttach(ND_LOCAL_NODE, display_pid, 1, _NTO_SIDE_CHANNEL, 0);
    if (coid == -1) {
        perror("ConnectAttach failed");
        return EXIT_FAILURE;
    }

    State current_state = IDLE;
    State (*state_handler)(Person*) = handle_idle;

    while (1) {
        rcvid = MsgReceive(child, &display_message, sizeof(display_message), NULL);
        if (rcvid == -1) {
            perror("Message Receiving failure");
            continue;
        }
        MsgReply(rcvid, EOK, NULL, 0);

        current_state = state_handler(&display_message);

        switch (current_state) {
            case IDLE:
                state_handler = handle_idle;
                break;
            case LEFT_SCAN:
                state_handler = handle_left_scan;
                break;
            case GUARD_LEFT_UNLOCK:
                state_handler = handle_guard_left_unlock;
                break;
            case LEFT_OPEN:
                state_handler = handle_left_open;
                break;
            case WEIGHT_SCAN:
                state_handler = handle_weight_scan;
                break;
            case LEFT_CLOSE:
                state_handler = handle_left_close;
                break;
            case GUARD_LEFT_LOCK:
                state_handler = handle_guard_left_lock;
                break;
            case GUARD_RIGHT_UNLOCK:
                state_handler = handle_guard_right_unlock;
                break;
            case RIGHT_OPEN:
                state_handler = handle_right_open;
                break;
            case RIGHT_CLOSE:
                state_handler = handle_right_close;
                break;
            case GUARD_RIGHT_LOCK:
                state_handler = handle_guard_right_lock;
                break;
            case RIGHT_SCAN:
                state_handler = handle_right_scan;
                break;
            case EXIT:
                state_handler = handle_exit;
                break;
            default:
                fprintf(stderr, "Invalid state\n");
                return EXIT_FAILURE;
                break;
        }
    }
    ChannelDestroy(child);
    ConnectDetach(coid);
    return EXIT_SUCCESS;
}

State handle_idle(Person* person) {
    if (person->current_state == LEFT_SCAN) {
        exiting = false;
        send_to_display(LEFT_SCAN_MSG);
        return LEFT_SCAN;
    } else if (person->current_state == RIGHT_SCAN) {
        exiting = true;
        send_to_display(RIGHT_SCAN_MSG);
        return RIGHT_SCAN;
    }
    return IDLE;
}

State handle_left_scan(Person* person) {
    if (person->current_state == GUARD_LEFT_UNLOCK) {
        send_to_display(GUARD_LEFT_UNLOCK_MSG);
        return GUARD_LEFT_UNLOCK;
    }
    return LEFT_SCAN;
}

State handle_guard_left_unlock(Person* person) {
    if (person->current_state == LEFT_OPEN) {
        send_to_display(LEFT_OPEN_MSG);
        return LEFT_OPEN;
    }
    return GUARD_LEFT_UNLOCK;
}

State handle_left_open(Person* person) {
    if (person->current_state == WEIGHT_SCAN && !exiting) {
        send_to_display(WEIGHT_UPDATE_MSG);
        return WEIGHT_SCAN;
    } else if (person->current_state == LEFT_CLOSE && exiting) {
        send_to_display(LEFT_CLOSE_MSG);
        return LEFT_CLOSE;
    }
    return LEFT_OPEN;
}

State handle_weight_scan(Person* person) {
    if (person->current_state == LEFT_CLOSE && !exiting) {
        send_to_display(LEFT_CLOSE_MSG);
        return LEFT_CLOSE;
    } else if (person->current_state == RIGHT_CLOSE && exiting) {
        send_to_display(RIGHT_CLOSE_MSG);
        return RIGHT_CLOSE;
    }
    return WEIGHT_SCAN;
}

State handle_left_close(Person* person) {
    if (person->current_state == GUARD_LEFT_LOCK) {
        send_to_display(GUARD_LEFT_LOCK_MSG);
        return GUARD_LEFT_LOCK;
    }
    return LEFT_CLOSE;
}

State handle_guard_left_lock(Person* person) {
    if (person->current_state == GUARD_RIGHT_UNLOCK && !exiting) {
        send_to_display(GUARD_RIGHT_UNLOCK_MSG);
        return GUARD_RIGHT_UNLOCK;
    } else if (person->current_state == EXIT && exiting) {
        send_to_display(EXIT_MSG);
        return EXIT;
    }
    return GUARD_LEFT_LOCK;
}

State handle_guard_right_unlock(Person* person) {
    if (person->current_state == RIGHT_OPEN) {
        send_to_display(RIGHT_OPEN_MSG);
        return RIGHT_OPEN;
    }
    return GUARD_RIGHT_UNLOCK;
}

State handle_right_open(Person* person) {
    if (person->current_state == RIGHT_CLOSE && !exiting) {
        send_to_display(RIGHT_CLOSE_MSG);
        return RIGHT_CLOSE;
    } else if (person->current_state == WEIGHT_SCAN && exiting) {
        send_to_display(WEIGHT_UPDATE_MSG);
        return WEIGHT_SCAN;
    }
    return RIGHT_OPEN;
}

State handle_right_close(Person* person) {
    if (person->current_state == GUARD_RIGHT_LOCK) {
        send_to_display(GUARD_RIGHT_LOCK_MSG);
        return GUARD_RIGHT_LOCK;
    }
    return RIGHT_CLOSE;
}

State handle_guard_right_lock(Person* person) {
    if (person->current_state == EXIT && !exiting) {
        send_to_display(EXIT_MSG);
        return EXIT;
    } else if (person->current_state == GUARD_LEFT_UNLOCK && exiting) {
        send_to_display(GUARD_LEFT_UNLOCK_MSG);
        return GUARD_LEFT_UNLOCK;
    }
    return GUARD_RIGHT_LOCK;
}

State handle_right_scan(Person* person) {
    if (person->current_state == GUARD_RIGHT_UNLOCK) {
        send_to_display(GUARD_RIGHT_UNLOCK_MSG);
        return GUARD_RIGHT_UNLOCK;
    }
    return RIGHT_SCAN;
}

State handle_exit(Person* person) {
    send_to_display(EXIT_MSG);
    return IDLE;
}
