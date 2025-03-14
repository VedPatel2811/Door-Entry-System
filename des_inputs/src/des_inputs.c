#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/neutrino.h>
#include <sys/netmgr.h>
#include <errno.h>
#include "../../des_controller/src/des-mva.h"

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <controller_pid>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t controller_pid = atoi(argv[1]);

    // Attach to the controller's channel
    int coid = ConnectAttach(ND_LOCAL_NODE, controller_pid, 1, _NTO_SIDE_CHANNEL, 0);
    if (coid == -1) {
        perror("ConnectAttach failed");
        exit(EXIT_FAILURE);
    }

    printf("Connected to controller with PID: %d\n", controller_pid);

    while (1) {
        char event[10];
        Person person;
        memset(&person, 0, sizeof(person));

        // Prompt for the event type
        printf("Enter the event type (ls=left scan, rs=right scan, ws=weight scale, lo=left open, ro=right open, lc=left closed, rc=right closed, gru=guard right unlock, grl=guard right lock, gll=guard left lock, glu=guard left unlock, exit): ");
        scanf("%s", event);

        if (strcmp(event, "exit") == 0) {
            // Exit the loop and terminate the program
            break;
        }

        // Handle different event types
        if (strcmp(event, "ls") == 0) {
            person.current_state = LEFT_SCAN;
            printf("Enter the person_id: ");
            scanf("%d", &person.person_id);
        } else if (strcmp(event, "rs") == 0) {
            person.current_state = RIGHT_SCAN;
            printf("Enter the person_id: ");
            scanf("%d", &person.person_id);
        } else if (strcmp(event, "ws") == 0) {
            person.current_state = WEIGHT_SCAN;
            printf("Enter the weight: ");
            scanf("%d", &person.weight);
        } else if (strcmp(event, "lo") == 0) {
            person.current_state = LEFT_OPEN;
        } else if (strcmp(event, "ro") == 0) {
            person.current_state = RIGHT_OPEN;
        } else if (strcmp(event, "lc") == 0) {
            person.current_state = LEFT_CLOSE;
        } else if (strcmp(event, "rc") == 0) {
            person.current_state = RIGHT_CLOSE;
        } else if (strcmp(event, "gru") == 0) {
            person.current_state = GUARD_RIGHT_UNLOCK;
        } else if (strcmp(event, "grl") == 0) {
            person.current_state = GUARD_RIGHT_LOCK;
        } else if (strcmp(event, "gll") == 0) {
            person.current_state = GUARD_LEFT_LOCK;
        } else if (strcmp(event, "glu") == 0) {
            person.current_state = GUARD_LEFT_UNLOCK;
        } else {
            fprintf(stderr, "Invalid event type: %s\n", event);
            continue;
        }

        // Send the Person object to the controller
        if (MsgSend(coid, &person, sizeof(person), NULL, 0) == -1) {
            perror("MsgSend failed");
            ConnectDetach(coid);
            exit(EXIT_FAILURE);
        }

        printf("Event sent to controller: %s\n", event);
    }

    // Detach from the controller's channel
    ConnectDetach(coid);
    printf("Disconnected from controller. Exiting...\n");

    return EXIT_SUCCESS;
}
