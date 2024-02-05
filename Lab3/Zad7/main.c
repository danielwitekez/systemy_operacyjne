#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

#define BUFFER_SIZE 100
#define NAMED_PIPE_PATH "./my_named_pipe"

void parentProcess(int pipefd[2]) {
    close(pipefd[0]); // Zamknięcie nieużywanego końca potoku nienazwanego

    char message[BUFFER_SIZE];
    printf("Wprowadź wiadomość od rodzica:\n");
    if (fgets(message, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    if (write(pipefd[1], message, strlen(message)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    close(pipefd[1]); // Zamknięcie końca potoku nienazwanego po zapisie
}

void childProcess() {
    int namedPipe = open(NAMED_PIPE_PATH, O_RDONLY);
    if (namedPipe == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char receivedMessage[BUFFER_SIZE];
    ssize_t bytesRead = read(namedPipe, receivedMessage, BUFFER_SIZE);

    if (bytesRead == -1) {
        perror("read");
        close(namedPipe);
        exit(EXIT_FAILURE);
    } else if (bytesRead == 0) {
        printf("Kolejka nazwana jest pusta.\n");
    } else {
        receivedMessage[bytesRead] = '\0';
        printf("Odczytana wiadomość od dziecka (kolejka nazwana): %s\n", receivedMessage);
    }

    close(namedPipe);
}

int main(void) {
    int pipefd[2]; // Tablica do przechowywania deskryptorów potoku nienazwanego

    // Tworzenie potoku nienazwanego
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Utworzenie kolejki nazwanej
    if (mkfifo(NAMED_PIPE_PATH, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    // Utworzenie procesu potomnego
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Kod dla procesu potomnego
        childProcess();
    } else {
        // Kod dla procesu rodzica
        parentProcess(pipefd);

        // Oczekiwanie na zakończenie procesu potomnego
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }

        // Usunięcie kolejki nazwanej
        if (unlink(NAMED_PIPE_PATH) == -1) {
            perror("unlink");
            exit(EXIT_FAILURE);
        }

        // Zamknięcie nieużywanych deskryptorów potoku nienazwanego
        close(pipefd[0]);
        close(pipefd[1]);
    }

    return 0;
}
