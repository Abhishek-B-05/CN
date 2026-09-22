#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 9002
#define BUF_SIZE 4096

int main() {
    int sock;
    struct sockaddr_in server_address;
    char filename[256];
    char response[BUF_SIZE];

    sock = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &server_address.sin_addr);

    if (connect(sock, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        perror("Connection failed");
        exit(1);
    }

    printf("Enter filename required: ");
    fgets(filename, sizeof(filename), stdin);
    filename[strcspn(filename, "\n")] = 0;

    send(sock, filename, strlen(filename), 0);

    memset(response, 0, BUF_SIZE);
    recv(sock, response, BUF_SIZE, 0);

    printf("%s\n", response);

    close(sock);
    return 0;
}
