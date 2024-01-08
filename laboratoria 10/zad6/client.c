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

void send_ordered_messages(int client_socket) {
    struct Packet packet;
    int messages_sent = 0;

    while (messages_sent < 10) {
        snprintf(packet.data, sizeof(packet.data), "Message %d", messages_sent + 1);
        packet.sequence_number = messages_sent;

        // Send a packet to the server
        if (sendto(client_socket, &packet, sizeof(packet), 0, (struct sockaddr*)NULL, sizeof(struct sockaddr)) == -1) {
            perror("sendto");
            exit(EXIT_FAILURE);
        }

        // Receive acknowledgment from the server indicating the expected sequence number
        int expected_sequence_number;
        ssize_t bytes_received = recv(client_socket, &expected_sequence_number, sizeof(expected_sequence_number), 0);
        if (bytes_received == -1) {
            perror("recv");
            exit(EXIT_FAILURE);
        }

        // Check if the acknowledgment is in order
        if (expected_sequence_number == messages_sent) {
            printf("Message acknowledged by the server.\n");
            messages_sent++;
        } else {
            // Retransmit the packet if acknowledgment is out of order
            printf("Out-of-order acknowledgment received. Retransmitting...\n");
        }
    }
}

int main() {
    int client_socket;
    struct sockaddr_in server_addr;

    // Creating socket
    client_socket = socket(AF_INET, SOCK_DGRAM, 0);
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

    // Send ordered messages to the server
    send_ordered_messages(client_socket);

    close(client_socket);

    return 0;
}
