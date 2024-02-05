#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define MESSAGE_SIZE 128
#define MESSAGE_TYPES 5

struct msg_buffer {
    long msg_type;
    char msg_text[MESSAGE_SIZE];
};

int compare(const void *a, const void *b) {
    return (*(long *)a - *(long *)b);
}

int main(void) {
    key_t key = ftok(".", 'A');
    int msg_queue_id;

    if ((msg_queue_id = msgget(key, IPC_CREAT | 0666)) == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    struct msg_buffer message;
    long msg_types[MESSAGE_TYPES];

    printf("Wybierz porządek sortowania (1 - rosnąco, 2 - malejąco): ");
    int order;
    scanf("%d", &order);

    // Odbieraj komunikaty
    for (int i = 0; i < 10; ++i) {
        if (msgrcv(msg_queue_id, &message, sizeof(message), 0, 0) == -1) {
            perror("msgrcv");
            exit(EXIT_FAILURE);
        }

        msg_types[i] = message.msg_type;
    }

    // Sortowanie komunikatów zgodnie z wyborem użytkownika
    if (order == 1) {
        qsort(msg_types, 10, sizeof(long), compare);
    } else if (order == 2) {
        qsort(msg_types, 10, sizeof(long), compare);
        for (int i = 0; i < 5; ++i) {
            long temp = msg_types[i];
            msg_types[i] = msg_types[9 - i];
            msg_types[9 - i] = temp;
        }
    } else {
        printf("Nieprawidłowy wybór porządku sortowania.\n");
        exit(EXIT_FAILURE);
    }

    // Wyświetl posortowane typy komunikatów
    printf("Posortowane typy komunikatów: ");
    for (int i = 0; i < 10; ++i) {
        printf("%ld ", msg_types[i]);
    }
    printf("\n");

    return 0;
}
