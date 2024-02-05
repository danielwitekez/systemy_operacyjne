CC = gcc
CFLAGS = -Wall

all: msg_sender msg_receiver

msg_sender: msg_sender.c
	$(CC) $(CFLAGS) msg_sender.c -o msg_sender

msg_receiver: msg_receiver.c
	$(CC) $(CFLAGS) msg_receiver.c -o msg_receiver

clean:
	rm -f msg_sender msg_receiver
