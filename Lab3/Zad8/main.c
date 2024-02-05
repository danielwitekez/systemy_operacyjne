#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#define FIFO_PATH "./my_fifo"
#define BUFFER_SIZE 100

void bidirectionalProcess() {
    int fifo = open(FIFO_PATH, O_RDWR);
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    while (1) {
        // Odbieranie wiadomości
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

        // Wprowadź wiadomość do wysłania
        char message[BUFFER_SIZE];
        printf("Wprowadź wiadomość do wysłania (wpisz 'exit' aby zakończyć):\n");
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
            perror("fgets");
            close(fifo);
            exit(EXIT_FAILURE);
        }

        // Sprawdź, czy użytkownik chce zakończyć
        if (strcmp(message, "exit\n") == 0) {
            break;
        }

        // Wysyłanie wiadomości
        if (write(fifo, message, strlen(message)) == -1) {
            perror("write");
            close(fifo);
            exit(EXIT_FAILURE);
        }
    }

    close(fifo);
}

int main(void) {
    if (mknod(FIFO_PATH, S_IFIFO | 0666, 0) == -1) {
        perror("mknod");
        exit(EXIT_FAILURE);
    }

    bidirectionalProcess();

    // Usunięcie kolejki FIFO po zakończeniu programu
    if (unlink(FIFO_PATH) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}
