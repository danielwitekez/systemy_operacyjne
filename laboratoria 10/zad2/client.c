#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define MSG_SIZE 128

void communicate_with_server() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MSG_SIZE];

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

    // Sending message to the server
    printf("Enter a message for the server: ");
    fgets(buffer, MSG_SIZE, stdin);
    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
        perror("send");
        exit(EXIT_FAILURE);
    }

    // Receiving acknowledgment from the server
    ssize_t bytes_received = recv(client_socket, buffer, sizeof(buffer), 0);
    if (bytes_received == -1) {
        perror("recv");
        exit(EXIT_FAILURE);
    }

    buffer[bytes_received] = '\0';
    printf("Server acknowledgment: %s\n", buffer);

    close(client_socket);
}

int main() {
    communicate_with_server();

    return 0;
}
