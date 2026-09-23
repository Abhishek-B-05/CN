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

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    if (server_socket < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    if (bind(server_socket, (struct sockaddr *)&server_address,
             sizeof(server_address)) < 0) {
        perror("Bind failed");
        close(server_socket);
        exit(1);
    }

    if (listen(server_socket, 5) < 0) {
        perror("Listen failed");
        close(server_socket);
        exit(1);
    }

    printf("Server ready, waiting for client requests...\n");

    while (1) {
        client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

        if (client_socket < 0) {
            perror("Accept failed");
            continue;
        }

        pid_t pid = fork();

        if (pid < 0) {
            perror("Fork failed");
            close(client_socket);
            continue;
        }

        if (pid == 0) {
            close(server_socket);
            char filename[256];
            char response[BUF_SIZE];
            while (1) {
                memset(filename, 0, sizeof(filename));
                int bytes_received = recv(client_socket,
                                          filename,
                                          sizeof(filename) - 1,
                                          0);
                if (bytes_received <= 0) {
                    printf("Client disconnected.\n");
                    break;
                }
                filename[bytes_received] = '\0';
                printf("Client requested: %s\n", filename);
                if (strcmp(filename, "quit") == 0) {
                    printf("Client requested to quit.\n");
                    break;
                }
                FILE *fp = fopen(filename, "r");
                memset(response, 0, sizeof(response));
                if (fp != NULL) {
                    char line[256];
                    char content[BUF_SIZE - 100];
                    memset(content, 0, sizeof(content));
                    while (fgets(line, sizeof(line), fp) != NULL) {
                        strcat(content, line);
                    }
                    snprintf(response, sizeof(response),"Server PID: %d\n%s", getpid(),content);
                    fclose(fp);
                } else {
                    snprintf(response,sizeof(response),"Server PID: %d\nFile not found",
                             getpid());
                }
                send(client_socket,
                     response,
                     strlen(response),
                     0);
            }
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
