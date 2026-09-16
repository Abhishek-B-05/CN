#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/select.h>

#define PORT 8080
#define BUF_SIZE 1024
#define MAX_CLIENTS 10

typedef struct {
    int socket;
    int id;
    int active;
} client_t;

client_t clients[MAX_CLIENTS];

void init_clients() {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        clients[i].socket = 0;
        clients[i].id = i + 1;
        clients[i].active = 0;
    }
}

void broadcast_message(char *message, int sender_id) {
    for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && clients[i].id != sender_id) {
            send(clients[i].socket, message, strlen(message), 0);
        }
    }
}

int main() {
    int server_socket, new_socket, max_sd, activity, sd;
    struct sockaddr_in server_address, client_address;
    socklen_t client_len = sizeof(client_address);
    char buffer[BUF_SIZE];
    fd_set readfds;

    init_clients();

    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    int opt = 1;
    setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    listen(server_socket, MAX_CLIENTS);

    printf("Server waiting for clients on port %d...\n", PORT);

    while (1) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        max_sd = server_socket;

        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socket;
            if (sd > 0) FD_SET(sd, &readfds);
            if (sd > max_sd) max_sd = sd;
        }

        activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            continue;
        }

        // New connection
        if (FD_ISSET(server_socket, &readfds)) {
            new_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_len);

            int assigned = 0;
            for (int i = 0; i < MAX_CLIENTS; i++) {
                if (!clients[i].active) {
                    clients[i].socket = new_socket;
                    clients[i].active = 1;
                    assigned = 1;

                    printf("Client %d connected: %s\n", clients[i].id, inet_ntoa(client_address.sin_addr));

                    char welcome[BUF_SIZE];
                    snprintf(welcome, BUF_SIZE, "Welcome! You are Client %d\n", clients[i].id);
                    send(new_socket, welcome, strlen(welcome), 0);

                    char notify[BUF_SIZE];
                    snprintf(notify, BUF_SIZE, "Client %d has joined the chat.\n", clients[i].id);
                    broadcast_message(notify, clients[i].id);
                    break;
                }
            }
            if (!assigned) {
                char *full_msg = "Server full. Try again later.\n";
                send(new_socket, full_msg, strlen(full_msg), 0);
                close(new_socket);
            }
        }

        // Handle messages from existing clients
        for (int i = 0; i < MAX_CLIENTS; i++) {
            sd = clients[i].socket;
            if (clients[i].active && FD_ISSET(sd, &readfds)) {
                memset(buffer, 0, BUF_SIZE);
                int n = recv(sd, buffer, BUF_SIZE - 1, 0);

                if (n <= 0) {
                    printf("Client %d disconnected.\n", clients[i].id);
                    close(sd);
                    clients[i].socket = 0;
                    clients[i].active = 0;

                    char notify[BUF_SIZE];
                    snprintf(notify, BUF_SIZE, "Client %d has left the chat.\n", clients[i].id);
                    broadcast_message(notify, -1);
                } else {
                    printf("Client %d: %s\n", clients[i].id, buffer);

                    char formatted[BUF_SIZE + 32];
                    snprintf(formatted, sizeof(formatted), "Client %d: %s\n", clients[i].id, buffer);
                    broadcast_message(formatted, clients[i].id);

                    if (strncmp(buffer, "exit", 4) == 0) {
                        close(sd);
                        clients[i].socket = 0;
                        clients[i].active = 0;
                    }
                }
            }
        }
    }

    close(server_socket);
    return 0;
}
