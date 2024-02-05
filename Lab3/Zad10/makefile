CC = gcc
CFLAGS = -Wall -Werror

all: fifo_sender fifo_receiver fifo_sum

fifo_sender: fifo_sender.c
	$(CC) $(CFLAGS) -o fifo_sender fifo_sender.c

fifo_receiver: fifo_receiver.c
	$(CC) $(CFLAGS) -o fifo_sendern fifo_sendern.c

fifo_sum: fifo_sum.c
	$(CC) $(CFLAGS) -o fifo_sum fifo_sum.c

clean:
	rm -f fifo_sender fifo_receiver fifo_sum
