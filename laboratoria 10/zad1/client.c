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
    int msgid = msgget(key, 0666);

    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    struct message msg;
    msg.mtype = 1;  // Use message type 1 for communication

    printf("Enter a message for the server: ");
    fgets(msg.mtext, MSG_SIZE, stdin);

    // Sending message to the server
    if (msgsnd(msgid, &msg, sizeof(msg.mtext), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }


    if (msgrcv(msgid, &msg, sizeof(msg.mtext), 2, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Server acknowledgment: %s\n", msg.mtext);

    return 0;
}
