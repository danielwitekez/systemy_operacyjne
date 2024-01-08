#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <signal.h>

#define PORT 8080
#define MSG_SIZE 128
#define MAX_RETRANSMISSIONS 3

volatile sig_atomic_t retransmission_required = 0;

void handle_alarm(int signum) {
    retransmission_required = 1;
}

void send_messages(int server_socket) {
    char buffer[MSG_SIZE];
    int messages_sent = 0;

    signal(SIGALRM, handle_alarm);

    while (messages_sent < 10) {
        snprintf(buffer, sizeof(buffer), "Message %d", messages_sent + 1);

        retransmission_required = 0;

        // Set an alarm for retransmission
        alarm(5); // 5 seconds timeout, adjust as needed

        // Send a message to the server
        if (send(server_socket, buffer, strlen(buffer) + 1, 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        // Cancel the alarm
        alarm(0);

        // Simulate packet loss for retransmission testing
        if (rand() % 2 == 0 && !retransmission_required) {
            printf("Simulated packet loss. Retransmitting...\n");
            continue;
        }

        // Wait for acknowledgment from the server
        char ack_buffer[3];
        ssize_t bytes_received = recv(server_socket, ack_buffer, sizeof(ack_buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        if (bytes_received == 0) {
            // Connection closed by the server
            printf("Server closed the connection.\n");
            break;
        }

        if (strcmp(ack_buffer, "ACK") == 0) {
            printf("Message acknowledged by the server.\n");
            messages_sent++;
        } else {
            printf("Invalid acknowledgment. Retransmitting...\n");
        }
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Creating socket
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initializing server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);

    // Converting IP address to binary form
    if (inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr) <= 0) {
        perror("inet_pton");
        exit(EXIT_FAILURE);
    }

    // Connecting to the server
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("connect");
        exit(EXIT_FAILURE);
    }

    printf("Connected to the server on port %d\n", PORT);

    // Send 10 messages to the server
    send_messages(client_socket);

    close(client_socket);

    return 0;
}
