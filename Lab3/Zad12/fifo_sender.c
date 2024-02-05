#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_PATH "./my_fifo"
#define MAX_NUMBER 10

void senderProcess() {
    int fifo = open(FIFO_PATH, O_WRONLY);
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= MAX_NUMBER; ++i) {
        printf("Wysyłanie liczby: %d\n", i);
        if (write(fifo, &i, sizeof(int)) == -1) {
            perror("write");
            close(fifo);
            exit(EXIT_FAILURE);
        }
        sleep(1); // Symulacja pracy
    }

    // Zamknij potok po zakończeniu wysyłania liczb
    close(fifo);
}

int main(void) {
    // Utwórz kolejkę FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    senderProcess();

    // Usuń kolejkę FIFO po zakończeniu programu
    if (unlink(FIFO_PATH) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}
