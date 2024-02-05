#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_PATH "./my_fifo"

void receiverProcess() {
    int fifo = open(FIFO_PATH, O_RDONLY);
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int received_number;

    while (read(fifo, &received_number, sizeof(int)) > 0) {
        printf("Odebrano liczbę: %d\n", received_number);
    }

    // Zamknij potok po zakończeniu odbierania liczb
    close(fifo);
}

int main(void) {
    // Utwórz kolejkę FIFO
    if (mkfifo(FIFO_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    receiverProcess();

    return 0;
}
