#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUF_SIZE 1024

int sock;

void *receive_handler(void *arg) {
    char buffer[BUF_SIZE];
    while (1) {
        memset(buffer, 0, BUF_SIZE);
        int n = recv(sock, buffer, BUF_SIZE - 1, 0);
        if (n <= 0) {
            printf("\nDisconnected from server.\n");
            exit(0);
        }
        printf("\r%s\nYou: ", buffer);
        fflush(stdout);
    }
    return NULL;
}

int main() {
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

    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_handler, NULL);

    while (1) {
        printf("You: ");
        fflush(stdout);
        fgets(buffer, BUF_SIZE, stdin);
        buffer[strcspn(buffer, "\n")] = 0;

        send(sock, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0)
            break;
    }

    close(sock);
    return 0;
}
