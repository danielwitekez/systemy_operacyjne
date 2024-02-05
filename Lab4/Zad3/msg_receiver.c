#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MESSAGE_SIZE 128

struct msg_buffer {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

int main(void) {
    key_t key = ftok(".", 'A');
    int msg_queue_id;

    if ((msg_queue_id = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    struct msg_buffer message;

    // Odbieraj komunikaty
    for (int i = 1; i <= 5; ++i) {
        // Spróbuj odebrać komunikat bez czekania (IPC_NOWAIT)
        if (msgrcv(msg_queue_id, &message, sizeof(message), i, IPC_NOWAIT) == -1) {
            perror("msgrcv (IPC_NOWAIT)");
        } else {
            printf("Odebrano komunikat o numerze %ld: %s\n", message.msg_type, message.msg_text);
        }

        // Odbierz komunikat z oczekiwaniem
        if (msgrcv(msg_queue_id, &message, sizeof(message), i, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        printf("Odebrano komunikat o numerze %ld: %s\n", message.msg_type, message.msg_text);
    }

    return 0;
}
