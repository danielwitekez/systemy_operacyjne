#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUF_SIZE 1024

void send_file(const char* file_path, int server_socket) {
    FILE *file;
    char buffer[BUF_SIZE];
    size_t bytes_read;

    // Opening the file for reading
    file = fopen(file_path, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    // Sending file data
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(server_socket, buffer, bytes_read, 0) == -1) {
            perror("send");
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);
    printf("File sent successfully.\n");
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

    // Send the file to the server
    send_file("file_to_send.txt", client_socket);

    close(client_socket);

    return 0;
}
