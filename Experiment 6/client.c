#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1234
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_address;
    socklen_t server_len = sizeof(server_address);
    char buffer[BUF_SIZE];

    sock = socket(AF_INET, SOCK_DGRAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    while (1) {
        printf("Enter message (or 'stop' to quit): ");
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        if (strcmp(buffer, "stop") == 0)
            break;

        sendto(sock, buffer, strlen(buffer), 0,
               (struct sockaddr *)&server_address, server_len);

        memset(buffer, 0, BUF_SIZE);
        int n = recvfrom(sock, buffer, BUF_SIZE, 0,
                          (struct sockaddr *)&server_address, &server_len);
        buffer[n] = '\0';

        printf("Server time: %s\n", buffer);
    }

    close(sock);
    return 0;
}
