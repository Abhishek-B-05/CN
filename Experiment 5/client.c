#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUF_SIZE 1024

int main() {
    int sock;
    struct sockaddr_in server_address;
    char buffer[BUF_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Connected to server successfully!\n");

    while (1) {
        printf("You: ");
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0)
            break;

        memset(buffer, 0, BUF_SIZE);
        recv(sock, buffer, BUF_SIZE, 0);

        printf("Server: %s\n", buffer);
    }

    close(sock);
    return 0;
}
