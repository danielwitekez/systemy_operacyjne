#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/wait.h>

#define MESSAGE_SIZE 128

struct msg_buffer {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

int createMessageQueue(void) {
    int msg_queue_id;

    if ((msg_queue_id = msgget(IPC_PRIVATE, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    return msg_queue_id;
}

void sendMessage(int msg_queue_id, long msg_type, const char *msg_text) {
    struct msg_buffer message;

    message.msg_type = msg_type;
    snprintf(message.msg_text, sizeof(message.msg_text), "%s", msg_text);

    if (msgsnd(msg_queue_id, &message, sizeof(message), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }
}

void receiveMessage(int msg_queue_id, long msg_type) {
    struct msg_buffer message;

    if (msgrcv(msg_queue_id, &message, sizeof(message), msg_type, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Proces otrzymał komunikat o numerze %ld: %s\n", message.msg_type, message.msg_text);
}

void removeMessageQueue(int msg_queue_id) {
    if (msgctl(msg_queue_id, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    int msg_queue_id = createMessageQueue();
    pid_t pid;

    if ((pid = fork()) == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        for (int i = 1; i <= 5; ++i) {
            receiveMessage(msg_queue_id, i);
        }

        removeMessageQueue(msg_queue_id);
        exit(EXIT_SUCCESS);
    } else {
        for (int i = 1; i <= 5; ++i) {
            printf("Proces macierzysty wysyła komunikat o numerze %d\n", i);
            sendMessag
