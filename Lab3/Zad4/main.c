#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 100

void parentProcess(int pipefd[2], int nonBlock) {
    close(pipefd[0]); // Zamknięcie nieużywanego końca potoku

    char message[BUFFER_SIZE];
    printf("Wprowadź wiadomość od rodzica:\n");
    if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    if (nonBlock) {
        if (fcntl(pipefd[1], F_SETFL, fcntl(pipefd[1], F_GETFL) | O_NONBLOCK) == -1) {
            perror("fcntl");
            exit(EXIT_FAILURE);
        }
    }

    if (write(pipefd[1], message, BUFFER_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(pipefd[1]); // Zamknięcie końca potoku po zapisie
}

void childProcess(int pipefd[2]) {
    close(pipefd[1]); // Zamknięcie nieużywanego końca potoku

    char receivedMessage[BUFFER_SIZE];
    ssize_t bytesRead = read(pipefd[0], receivedMessage, BUFFER_SIZE);

    if (bytesRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    } else if (bytesRead == 0) {
        printf("Potok jest pusty.\n");
    } else {
        printf("Odczytana wiadomość od dziecka: %s\n", receivedMessage);
    }

    close(pipefd[0]); // Zamknięcie końca potoku po odczycie
}

int main(int argc, char *argv[]) {
    int pipefd[2]; // Tablica do przechowywania deskryptorów potoku

    // Tworzenie potoku
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    if (argc == 2 && strcmp(argv[1], "-nb") == 0) {
        // Ustawienie flagi O_NONBLOCK
        parentProcess(pipefd, 1);
    } else {
        // Domyślne ustawienie bez flagi O_NONBLOCK
        parentProcess(pipefd, 0);
    }

    // Tworzenie procesu potomnego
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Kod dla procesu potomnego
        childProcess(pipefd);
    } else {
        // Kod dla procesu rodzica
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        // Zamknięcie nieużywanych deskryptorów potoku
        if (close(pipefd[0]) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }

        if (close(pipefd[1]) == -1) {
            perror("close");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
