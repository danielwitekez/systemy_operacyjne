#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

struct Packet {
    int sequence_number;
    char data[BUF_SIZE];
};

void receive_ordered_messages(int server_socket) {
    struct Packet buffer;
    ssize_t bytes_received;
    int expected_sequence_number = 0;

    while (1) {
        bytes_received = recv(server_socket, &buffer, sizeof(buffer), 0);
        if (bytes_received == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        if (bytes_received == 0) {
            // Connection closed by the client
            printf("Client closed the connection.\n");
            break;
        }

        // Check if the received packet is in order
        if (buffer.sequence_number == expected_sequence_number) {
            printf("Received message from client: %s\n", buffer.data);

            // Increment the expected sequence number
            expected_sequence_number++;

            // Send acknowledgment to the client
            if (sendto(server_socket, &expected_sequence_number, sizeof(expected_sequence_number), 0, (struct sockaddr*)NULL, sizeof(struct sockaddr)) == -1) {
                perror("sendto");
                exit(EXIT_FAILURE);
            }
        } else {
            // Out-of-order packet received, ignore and request retransmission
            printf("Out-of-order packet received. Requesting retransmission...\n");

            // Send acknowledgment to the client indicating the last correctly received packet
            if (sendto(server_socket, &expected_sequence_number, sizeof(expected_sequence_number), 0, (struct sockaddr*)NULL, sizeof(struct sockaddr)) == -1) {
                perror("sendto");
                exit(EXIT_FAILURE);
            }
        }
    }
}

int main() {
    int server_socket;
    struct sockaddr_in server_addr;

    // Creating socket
    server_socket = socket(AF_INET, SOCK_DGRAM, 0);
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

    printf("Server is listening on port %d...\n", PORT);

    // Receive ordered messages from the client
    receive_ordered_messages(server_socket);

    close(server_socket);

    return 0;
}
