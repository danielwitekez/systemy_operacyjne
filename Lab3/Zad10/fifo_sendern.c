#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_PATH "./my_fifo"
#define MAX_NUMBER 100

void receiverProcess() {
    int fifo = open(FIFO_PATH, O_WRONLY);
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i <= MAX_NUMBER; i += 2) {
        printf("Wysyłanie liczby nieparzystej: %d\n", i);
        if (write(fifo, &i, sizeof(int)) == -1) {
            perror("write");
            close(fifo);
            exit(EXIT_FAILURE);
        }
        sleep(1); // Symulacja pracy
    }

    if(close(fifo)<0)
      perror("close");
}

int main(void) {
    receiverProcess();
    return 0;
}
