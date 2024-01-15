#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <time.h>

#define SERVER_PORT 1096
#define SERVER_IP_ADDRESS "127.0.0.1"
#define MAX_MSG_SIZE 1024
#define NUM_MESSAGES 10

int server_socket;

void send_receive_messages(int socket_descriptor) {
    struct in_addr network_address;

    if (!inet_aton(SERVER_IP_ADDRESS, &network_address))
        perror("inet_aton");

    struct sockaddr_in server_address = {
        .sin_family = AF_INET,
        .sin_port = htons(SERVER_PORT),
        .sin_addr = network_address
    };

    for (int i = 0; i < NUM_MESSAGES; ++i) {
        const char *message_send = "Komunikat przesłany przez serwer.";
        if (sendto(socket_descriptor, message_send, strlen(message_send), 0,
                   (struct sockaddr *)&server_address, sizeof(server_address)) < 0)
            perror("sendto");

        // Simulate packet loss by generating a random number
        srand(time(NULL));
        int random_num = rand() % 10; // Adjust the range as needed

        if (random_num < 3) {
            printf("Simulating packet loss. Resending last message.\n");
            continue; // Skip the rest of the loop and resend the message
        }

        char buffer[MAX_MSG_SIZE];
        struct sockaddr_in client_address;
        socklen_t address_length = sizeof(client_address);

        int received_bytes = recvfrom(socket_descriptor, (void *)buffer, sizeof(buffer),
                                      0, (struct sockaddr*)&client_address, &address_length);
        if (received_bytes < 0)
            perror("recvfrom");
        else {
            buffer[received_bytes] = '\0';
            printf("Received from client: %s\n", buffer);
        }
    }
}

int main(void) {
    int socket_descriptor = socket(AF_INET, SOCK_DGRAM, 0);
    if (socket_descriptor < 0)
        perror("socket");

    send_receive_messages(socket_descriptor);

    if (close(socket_descriptor) < 0)
        perror("close");

    return 0;
}
