#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <process.h>
#include <unistd.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <ctype.h>
#include <errno.h>

#include "des-mva.h"

void show_usage(void);

void* ready_state();
void* left_scan();
void* right_scan();
void* weight_measure();
void* left_open();
void* right_open();
void* left_close();
void* right_close();
void* guard_right_lock();
void* guard_right_unlock();
void* guard_left_lock();
void* guard_left_unlock();
void* terminate_state();

ControllerResponse response;
Person person_message;
int connection;
StateFunction state_function = ready_state;

int main(int argc, char *argv[]) {

    pid_t display_pid;
    int channel_id, received_msg_id;

    if (argc != 2) {
        fprintf(stderr, "ERROR: Please match the arguments as requested.\n");
        show_usage();
        exit(EXIT_FAILURE);
    }

    display_pid = atoi(argv[1]);

    // Creating communication channel
    channel_id = ChannelCreate(0);
    if (channel_id == -1) {
        fprintf(stderr, "ERROR: Could not create channel.\n");
        exit(EXIT_FAILURE);
    }

    connection = ConnectAttach(ND_LOCAL_NODE, display_pid, 1, _NTO_SIDE_CHANNEL, 0);
    if (connection == -1) {
        fprintf(stderr, "ERROR: Could not connect to the des_display. Did you specify the right PID?\n");
        exit(EXIT_FAILURE);
    }

    printf("The controller is running as PID: %d \n", getpid());
    person_message.person_id = 0;
    person_message.weight = 0;
    person_message.current_state = IDLE;

    while (state_function != NULL) {
        received_msg_id = MsgReceive(channel_id, (void*) &person_message, sizeof(person_message), NULL);

        if (person_message.current_state == EXIT) {
            terminate_state();
        }

        state_function = (StateFunction) (*state_function)();

        // Send response back to sender
        response.status_code = EOK;
        MsgReply(received_msg_id, EOK, &response, sizeof(response));

        if (person_message.current_state == EXIT) {
            break;
        } else if (person_message.current_state == GUARD_RIGHT_LOCK || person_message.current_state == GUARD_LEFT_LOCK) {
            person_message.person_id = 0;
            person_message.weight = 0;
            person_message.current_state = IDLE;
        }
    }

    ChannelDestroy(channel_id);
    ConnectDetach(connection);
    return EXIT_SUCCESS;
}

void show_usage(void) {
    printf("   USAGE:\n");
    printf("./des_controller <display-pid>\n");
}

void* ready_state() {
    if (person_message.current_state == LEFT_SCAN) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return left_scan;
    } else if (person_message.current_state == RIGHT_SCAN) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return right_scan;
    }
    return ready_state;
}

void* left_scan() {
    if (person_message.current_state == GUARD_LEFT_UNLOCK) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return guard_left_unlock;
    }
    return left_scan;
}

void* right_scan() {
    if (person_message.current_state == GUARD_RIGHT_UNLOCK) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return guard_right_unlock;
    }
    return right_scan;
}

void* weight_measure() {
    if (person_message.current_state == LEFT_CLOSE) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return left_close;
    } else if (person_message.current_state == RIGHT_CLOSE) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return right_close;
    }
    return weight_measure;
}

void* left_open() {
    if (person_message.current_state == WEIGHT_SCAN) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return weight_measure;
    } else if (person_message.current_state == LEFT_CLOSE) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return left_close;
    }
    return left_open;
}

void* right_open() {
    if (person_message.current_state == WEIGHT_SCAN) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return weight_measure;
    } else if (person_message.current_state == RIGHT_CLOSE) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return right_close;
    }
    return right_open;
}

void* left_close() {
    if (person_message.current_state == GUARD_LEFT_LOCK) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return guard_left_lock;
    }
    return left_close;
}

void* right_close() {
    if (person_message.current_state == GUARD_RIGHT_LOCK) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return guard_right_lock;
    }
    return right_close;
}

void* guard_right_lock() {
    if (person_message.current_state == GUARD_LEFT_UNLOCK) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return guard_left_unlock;
    }
    return guard_right_lock;
}

void* guard_right_unlock() {
    if (person_message.current_state == RIGHT_OPEN) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return right_open;
    }
    return guard_right_unlock;
}

void* guard_left_lock() {
    if (person_message.current_state == GUARD_RIGHT_UNLOCK) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return guard_right_unlock;
    }
    return guard_left_lock;
}

void* guard_left_unlock() {
    if (person_message.current_state == LEFT_OPEN) {
        if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
            fprintf(stderr, "ERROR: Could not send message.\n");
            exit(EXIT_FAILURE);
        }
        return left_open;
    }
    return guard_left_unlock;
}

void* terminate_state() {
    person_message.current_state = EXIT;

    // Send the terminate message to des_display
    if (MsgSend(connection, &person_message, sizeof(person_message), &response, sizeof(response)) == -1) {
        fprintf(stderr, "ERROR: Could not send terminate message.\n");
        exit(EXIT_FAILURE);
    }

    // Print termination status
    printf("Controller: Terminating...\n");

    // Break out of the loop by returning NULL
    return NULL;
}
