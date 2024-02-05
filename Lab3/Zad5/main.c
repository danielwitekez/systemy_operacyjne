#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

#define BUFFER_SIZE 100

void parentProcess(int pipeParentToChild[2], int pipeChildToParent[2]) {
    close(pipeParentToChild[0]); // Zamknięcie nieużywanego końca potoku od rodzica do potomka
    close(pipeChildToParent[1]); // Zamknięcie nieużywanego końca potoku od potomka do rodzica

    char messageToChild[BUFFER_SIZE];
    char receivedMessageFromChild[BUFFER_SIZE];

    // Wprowadź wiadomość od rodzica
    printf("Wprowadź wiadomość od rodzica:\n");
    if (fgets(messageToChild, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    // Zapisz wiadomość do potoku od rodzica do potomka
    if (write(pipeParentToChild[1], messageToChild, BUFFER_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Odczytaj wiadomość od potomka do rodzica
    if (read(pipeChildToParent[0], receivedMessageFromChild, BUFFER_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("Odczytana wiadomość od potomka: %s\n", receivedMessageFromChild);

    // Zamknij nieużywane deskryptory potoków
    if (close(pipeParentToChild[1]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (close(pipeChildToParent[0]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
}

void childProcess(int pipeParentToChild[2], int pipeChildToParent[2]) {
    close(pipeParentToChild[1]); // Zamknięcie nieużywanego końca potoku od rodzica do potomka
    close(pipeChildToParent[0]); // Zamknięcie nieużywanego końca potoku od potomka do rodzica

    char receivedMessageFromParent[BUFFER_SIZE];
    char messageToParent[BUFFER_SIZE];

    // Odczytaj wiadomość od rodzica
    if (read(pipeParentToChild[0], receivedMessageFromParent, BUFFER_SIZE) == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    printf("Odczytana wiadomość od rodzica: %s\n", receivedMessageFromParent);

    // Wprowadź wiadomość od potomka
    printf("Wprowadź wiadomość od potomka:\n");
    if (fgets(messageToParent, BUFFER_SIZE, stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }

    // Zapisz wiadomość do potoku od potomka do rodzica
    if (write(pipeChildToParent[1], messageToParent, BUFFER_SIZE) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Zamknij nieużywane deskryptory potoków
    if (close(pipeParentToChild[0]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
    if (close(pipeChildToParent[1]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int pipeParentToChild[2]; // Potok od rodzica do potomka
    int pipeChildToParent[2]; // Potok od potomka do rodzica

    // Utwórz potoki
    if (pipe(pipeParentToChild) == -1 || pipe(pipeChildToParent) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Utwórz proces potomny
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Kod dla procesu potomnego
        childProcess(pipeParentToChild, pipeChildToParent);
    } else {
        // Kod dla procesu rodzica
        parentProcess(pipeParentToChild, pipeChildToParent);

        // Czekaj na zakończenie procesu potomnego
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
