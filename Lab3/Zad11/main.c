#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

#define READ_END 0
#define WRITE_END 1

void parentProcess(int pipe_fd[2]) {
    // Zamknij nieużywany koniec potoku
    if (close(pipe_fd[READ_END]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    // Wysyłaj liczby do potomka
    for (int i = 1; i <= 10; ++i) {
        printf("Proces macierzysty wysyła liczbę: %d\n", i);
        if (write(pipe_fd[WRITE_END], &i, sizeof(int)) == -1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
        sleep(1); // Symulacja pracy
    }

    // Zamknij używany koniec potoku
    if (close(pipe_fd[WRITE_END]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    // Oczekuj na zakończenie procesu potomnego
    if (wait(NULL) == -1) {
        perror("wait");
        exit(EXIT_FAILURE);
    }
}

void childProcess(int pipe_fd[2]) {
    // Zamknij nieużywany koniec potoku
    if (close(pipe_fd[WRITE_END]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }

    // Odczytuj liczby z potoku i wyświetlaj na ekranie
    int received_number;
    while (read(pipe_fd[READ_END], &received_number, sizeof(int)) > 0) {
        printf("Proces potomny otrzymał liczbę: %d\n", received_number);
    }

    // Zamknij używany koniec potoku
    if (close(pipe_fd[READ_END]) == -1) {
        perror("close");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    int pipe_fd[2];

    // Utwórz potok
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid_t pid = fork();

    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Kod dla procesu potomnego
        childProcess(pipe_fd);
        exit(EXIT_SUCCESS);
    } else {
        // Kod dla procesu macierzystego
        parentProcess(pipe_fd);
    }

    return 0;
}
