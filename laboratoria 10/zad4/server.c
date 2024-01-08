#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

void receive_file(int server_socket) {
    FILE *file;
    char buffer[BUF_SIZE];
    ssize_t bytes_received;
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(struct sockaddr_in);

    // Opening the file for writing
    file = fopen("received_file.txt", "wb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Receiving file data
    while (1) {
        bytes_received = recvfrom(server_socket, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &addr_len);
        if (bytes_received == -1) {
            perror("recvfrom");
            exit(EXIT_FAILURE);
        }

        if (bytes_received == 0) {
            break; // End of file
        }

        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
    printf("File received successfully.\n");
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

    // Receive the file from the client
    receive_file(server_socket);

    close(server_socket);

    return 0;
}
