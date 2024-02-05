#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 100

void receiverProcess(const char *fifoPath, int nonBlock) {
    int fifo = open(fifoPath, O_RDONLY | (nonBlock ? O_NONBLOCK : 0));
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char receivedMessage[BUFFER_SIZE];
    ssize_t bytesRead = read(fifo, receivedMessage, BUFFER_SIZE);

    if (bytesRead == -1) {
        perror("read");
        close(fifo);
        exit(EXIT_FAILURE);
    } else if (bytesRead == 0) {
        printf("Kolejka FIFO jest pusta.\n");
    } else {
        receivedMessage[bytesRead] = '\0';
        printf("Odebrana wiadomość: %s\n", receivedMessage);
    }

    close(fifo);
}

int main(int argc, char *argv[]) {
    const char *fifoPath = "./my_fifo";
    int nonBlock = 0; // Domyślnie nie używa flagi O_NONBLOCK

    if (argc == 2) {
        nonBlock = atoi(argv[1]);
    }

    receiverProcess(fifoPath, nonBlock);

    return 0;
}
