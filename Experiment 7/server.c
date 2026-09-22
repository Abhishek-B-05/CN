#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/wait.h>

#define PORT 9002
#define BUF_SIZE 4096

int main() {
    int server_socket, client_socket;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);
    char filename[256];
    char response[BUF_SIZE];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, 5);

    printf("Server ready, waiting for client requests...\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

        pid_t pid = fork();

        if (pid == 0) {
            close(server_socket);

            memset(filename, 0, sizeof(filename));
            recv(client_socket, filename, sizeof(filename), 0);

            FILE *fp = fopen(filename, "r");
            memset(response, 0, BUF_SIZE);

            if (fp != NULL) {
                char line[256];
                char content[BUF_SIZE - 100];
                memset(content, 0, sizeof(content));

                while (fgets(line, sizeof(line), fp) != NULL) {
                    strcat(content, line);
                }

                snprintf(response, BUF_SIZE, "Server PID: %d\n%s", getpid(), content);
                fclose(fp);
            } else {
                snprintf(response, BUF_SIZE, "Server PID: %d\nFile not found", getpid());
            }

            send(client_socket, response, strlen(response), 0);

            close(client_socket);
            exit(0);
        } else {
            close(client_socket);
            waitpid(-1, NULL, WNOHANG);
        }
    }

    close(server_socket);
    return 0;
}
