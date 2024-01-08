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

void receive_messages(int client_socket) {
    char buffer[MSG_SIZE];
    ssize_t bytes_received;
    int messages_received = 0;

    signal(SIGALRM, handle_alarm);

    while (messages_received < 10) {
        retransmission_required = 0;

        // Set an alarm for retransmission
        alarm(5); // 5 seconds timeout, adjust as needed

        bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        if (bytes_received == 0) {
            // Connection closed by the client
            printf("Client closed the connection.\n");
            break;
        }

        // Cancel the alarm
        alarm(0);

        // Process the received message
        printf("Received message from client: %s\n", buffer);

        // Simulate packet loss for retransmission testing
        if (rand() % 2 == 0 && !retransmission_required) {
            printf("Simulated packet loss. Retransmitting...\n");
            continue;
        }

        // Send acknowledgment to the client
        if (send(client_socket, "ACK", 3, 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }

        messages_received++;
    }
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // Creating socket
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    // Initializing server address structure
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Binding the socket
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("bind");
        exit(EXIT_FAILURE);
    }

    // Listening for incoming connections
    if (listen(server_socket, 5) == -1) {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    printf("Server is listening on port %d...\n", PORT);

    // Accepting incoming connection
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &addr_len);
    if (client_socket == -1) {
        perror("accept");
        exit(EXIT_FAILURE);
    }

    printf("Accepted connection from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Receive 10 messages from the client
    receive_messages(client_socket);

    close(client_socket);
    close(server_socket);

    return 0;
}
