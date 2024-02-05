#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>

#define NUM_PROCESSES 4
#define BUFFER_SIZE 10

void childProcess(int read_fd, int write_fd, int process_number) {
    int numbers[NUM_PROCESSES];
    ssize_t bytesRead;

    // Odbieranie liczb od poprzedniego procesu
    bytesRead = read(read_fd, numbers, sizeof(numbers));
    if (bytesRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    // Zwiększenie liczb o 1
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        numbers[i]++;
    }

    // Wysyłanie liczb do następnego procesu
    if (write(write_fd, numbers, sizeof(numbers)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Wypisanie liczb, jeśli to ostatni proces
    if (process_number == NUM_PROCESSES - 1) {
        printf("Ostateczne wartości: %d, %d, %d, %d\n", numbers[0], numbers[1], numbers[2], numbers[3]);
    }
}

int main(void) {
    int pipes[NUM_PROCESSES - 1][2]; // Tablica do przechowywania deskryptorów potoków
    pid_t pids[NUM_PROCESSES];        // Tablica do przechowywania identyfikatorów procesów

    // Utworzenie potoków między procesami
    for (int i = 0; i < NUM_PROCESSES - 1; ++i) {
        if (pipe(pipes[i]) == -1) {
            perror("pipe");
            exit(EXIT_FAILURE);
        }
    }

    // Utworzenie procesów
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        pids[i] = fork();

        if (pids[i] == -1) {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pids[i] == 0) {
            // Kod dla procesu potomnego
            if (i > 0) {
                // Zamknięcie nieużywanych końcówek potoków
                close(pipes[i - 1][1]);
                close(pipes[i][0]);
            }
            childProcess(pipes[i - 1][0], pipes[i][1], i);
            exit(EXIT_SUCCESS);
        }
    }

    // Kod dla procesu rodzica
    // Zamknięcie nieużywanych końcówek potoków
    close(pipes[0][0]);
    close(pipes[NUM_PROCESSES - 2][1]);

    // Inicjalizacja danych dla pierwszego procesu
    int initialNumbers[NUM_PROCESSES] = {1, 2, 3, 4};

    // Wysyłanie liczb do pierwszego procesu
    if (write(pipes[0][1], initialNumbers, sizeof(initialNumbers)) == -1) {
        perror("write");
        exit(EXIT_FAILURE);
    }

    // Oczekiwanie na zakończenie wszystkich procesów potomnych
    for (int i = 0; i < NUM_PROCESSES; ++i) {
        if (wait(NULL) == -1) {
            perror("wait");
            exit(EXIT_FAILURE);
        }
    }

    return 0;
}
