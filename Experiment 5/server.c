#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 8080
#define BUF_SIZE 1024

void handle_client(int client_socket) {
    char buffer[BUF_SIZE];
    int n;

    while (1) {
        memset(buffer, 0, BUF_SIZE);
        n = recv(client_socket, buffer, BUF_SIZE, 0);
        if (n <= 0) {
            printf("Client disconnected.\n");
            break;
        }
        printf("Received: %s\n", buffer);

        // Echo back to client
        send(client_socket, buffer, strlen(buffer), 0);

        if (strcmp(buffer, "exit") == 0)
            break;
    }
    close(client_socket);
}

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);

    printf("Server listening on port %d...\n", PORT);

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);
        printf("New client connected: %s\n", inet_ntoa(client_address.sin_addr));

        pid_t pid = fork();

        if (pid == 0) {
            // Child process handles this client
            close(server_socket);       // child doesn't need listening socket
            handle_client(client_socket);
            exit(0);
        } else {
            // Parent process
            close(client_socket);       // parent doesn't need this client's socket
            waitpid(-1, NULL, WNOHANG); // clean up finished children (non-blocking)
        }
    }

    close(server_socket);
    return 0;
}
