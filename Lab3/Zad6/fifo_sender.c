#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define BUFFER_SIZE 100

void senderProcess(const char *fifoPath, int nonBlock) {
    int fifo = open(fifoPath, O_WRONLY | (nonBlock ? O_NONBLOCK : 0));
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char message[BUFFER_SIZE];
    printf("Wprowadź wiadomość do wysłania:\n");
    if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets");
        close(fifo);
        exit(EXIT_FAILURE);
    }

    if (write(fifo, message, strlen(message)) == -1) {
        perror("write");
        close(fifo);
        exit(EXIT_FAILURE);
    }

    close(fifo);
}

int main(int argc, char *argv[]) {
    const char *fifoPath = "./my_fifo";
    int nonBlock = 0; // Domyślnie nie używa flagi O_NONBLOCK

    if (argc == 2) {
        nonBlock = atoi(argv[1]);
    }

    if (mkfifo(fifoPath, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    senderProcess(fifoPath, nonBlock);

    if (unlink(fifoPath) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}
