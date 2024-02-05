#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

#define BUFFER_SIZE 100

void createAndWriteToPipe(int pipefd[2], const char *data) {
    if (write(pipefd[1], data, BUFFER_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

void readFromPipe(int pipefd[2], char *buffer) {
    if (read(pipefd[0], buffer, BUFFER_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    int pipefd[2]; // Tablica do przechowywania deskryptorów potoku
    char buffer[BUFFER_SIZE];

    // Tworzenie potoku
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Wprowadź dowolny tekst
    printf("Wprowadź dowolny tekst:\n");
    if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    // Wywołanie funkcji do zapisu do potoku
    createAndWriteToPipe(pipefd, buffer);

    // Wywołanie funkcji do odczytu z potoku
    readFromPipe(pipefd, buffer);

    // Wyświetlanie danych
    printf("Odczytane dane z potoku: %s\n", buffer);

    // Zamknięcie deskryptorów potoku
    if (close(pipefd[0]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    if (close(pipefd[1]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    return 0;
}
