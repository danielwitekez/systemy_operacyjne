#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#define FILENAME "main.c"
#define MAX_CONTENT_SIZE 50
#define FIFO_NAME "sorted_fifo"

void runSortCommand() {
    // Wywołanie polecenia sort i przekierowanie wyników do lacza nazwanego
    if (system("sort " FILENAME " > " FIFO_NAME) == -1) {
        perror("system");
        exit(EXIT_FAILURE);
    }
}

void readAndPrintFirst50Chars() {
    // Otwarcie lacza nazwanego do odczytu
    int fifo = open(FIFO_NAME, O_RDONLY);
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    char content[MAX_CONTENT_SIZE + 1]; // Dodatkowy jeden znak na znak końca stringa
    ssize_t bytesRead = read(fifo, content, MAX_CONTENT_SIZE);
    if (bytesRead == -1) {
        perror("read");
        exit(EXIT_FAILURE);
    }

    content[bytesRead] = '\0'; // Dodanie znaku końca stringa
    printf("Pierwsze 50 znaków po sortowaniu: %s\n", content);

    // Zamknięcie lacza nazwanego
    if(close(fifo)<0)
        perror(close);
}

int main(void) {
    // Utworzenie lacza nazwanego
    if (mkfifo(FIFO_NAME, 0666) == -1) {
        perror("mkfifo");
        exit(EXIT_FAILURE);
    }

    runSortCommand();
    readAndPrintFirst50Chars();

    // Usunięcie lacza nazwanego po użyciu
    if (unlink(FIFO_NAME) == -1) {
        perror("unlink");
        exit(EXIT_FAILURE);
    }

    return 0;
}
