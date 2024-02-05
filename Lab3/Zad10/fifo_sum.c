#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_PATH "./my_fifo"
#define MAX_NUMBER 100

void sumProcess() {
    int fifo = open(FIFO_PATH, O_RDONLY);
    if (fifo == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    int sum = 0;
    int receivedNumber;

    while (read(fifo, &receivedNumber, sizeof(int)) > 0) {
        printf("Odebrano liczbę: %d\n", receivedNumber);
        sum += receivedNumber;
    }

    printf("Suma odebranych liczb: %d\n", sum);

    if(close(fifo)<0)
      perror("close");
}

int main(void) {
    sumProcess();
    return 0;
}
