#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 1234
#define BUF_SIZE 1024

int main() {
    int server_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);
    char buffer[BUF_SIZE];

    server_socket = socket(AF_INET, SOCK_DGRAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));

    printf("Server waiting for client requests...\n");

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int n = recvfrom(server_socket, buffer, BUF_SIZE, 0,
                          (struct sockaddr *)&client_address, &client_len);
        buffer[n] = '\0';

        printf("Received: %s\n", buffer);

        if (strcmp(buffer, "stop") == 0)
            break;

        time_t now = time(NULL);
        struct tm *local_time = localtime(&now);

        char time_str[BUF_SIZE];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", local_time);

        sendto(server_socket, time_str, strlen(time_str), 0,
               (struct sockaddr *)&client_address, client_len);
    }

    close(server_socket);
    return 0;
}
