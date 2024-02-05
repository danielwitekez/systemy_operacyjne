#include <stdio.h>
#include <stdlib.h>

#define MAX_INPUT_SIZE 1000

void getInput(char *input) {
    printf("Podaj dowolny ciąg znaków:\n");
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
        perror("fgets");
        exit(EXIT_FAILURE);
    }
}

void runWcCommand(const char *input) {
    // Uruchomienie polecenia wc za pomocą popen
    FILE *pipe = popen("wc", "w");
    if (pipe == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    // Przekazanie ciągu znaków na standardowe wejście polecenia wc
    if (fputs(input, pipe) == EOF) {
        perror("fputs");
        exit(EXIT_FAILURE);
    }

    // Zamknięcie potoku
    if (pclose(pipe) == -1) {
        perror("pclose");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    char input[MAX_INPUT_SIZE];
    getInput(input);
    runWcCommand(input);

    return 0;
}
