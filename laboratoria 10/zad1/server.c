#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/ipc.h>

#define MSG_SIZE 128

struct message {
    long mtype;
    char mtext[MSG_SIZE];
};

int main() {
    key_t key = ftok("server", 'S');
    int msgid = msgget(key, IPC_CREAT | 0666);

    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    struct message msg;

    while (1) {
        if (msgrcv(msgid, &msg, sizeof(msg.mtext), 1, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        printf("Received message from client: %s\n", msg.mtext);

        // Sending acknowledgment to the client
        msg.mtype = 2;  // Use a different message type for acknowledgment
        strcpy(msg.mtext, "Message received successfully!");
        
        if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
