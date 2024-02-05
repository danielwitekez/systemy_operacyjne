#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <time.h>

#define MESSAGE_SIZE 128
#define MESSAGE_TYPES 5

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
    srand(time(NULL));

    // Wysyłaj losowe komunikaty
    for (int i = 0; i < 10; ++i) {
        message.msg_type = rand() % MESSAGE_TYPES + 1;
        sprintf(message.msg_text, "To jest komunikat numer %d", i);

        if (msgsnd(msg_queue_id, &message, sizeof(message), 0) == -1) {
            perror("msgsnd");
            exit(EXIT_FAILURE);
        }

        sleep(1); // Symulacja pracy
    }

    return 0;
}
