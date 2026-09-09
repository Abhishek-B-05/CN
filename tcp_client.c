#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>

void send_all(int sock, void *buf, int len) {
    int sent = 0;
    while (sent < len) {
        int n = send(sock, (char *)buf + sent, len - sent, 0);
        if (n <= 0) { perror("send failed"); exit(1); }
        sent += n;
    }
}

void recv_all(int sock, void *buf, int len) {
    int received = 0;
    while (received < len) {
        int n = recv(sock, (char *)buf + received, len - received, 0);
        if (n <= 0) { perror("recv failed"); exit(1); }
        received += n;
    }
}

int main() {
    int client_socket;
    int n, i, j;
    int matrix[50][50];
    char type[50];
    char choice;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(9090);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    int ret = connect(client_socket, (struct sockaddr *)&server_address, sizeof(server_address));
    if (ret < 0) {
        perror("connect failed");
        exit(1);
    }
    printf("Connected to server!\n");

    do {
        printf("\nOrder of the matrix: ");
        scanf("%d", &n);
        send_all(client_socket, &n, sizeof(n));

        printf("\nEnter Matrix Elements:\n");
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                scanf("%d", &matrix[i][j]);
            }
        }

        printf("\nEntered Matrix:\n");
        for (i = 0; i < n; i++) {
            for (j = 0; j < n; j++) {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }

        for (i = 0; i < n; i++) {
            send_all(client_socket, matrix[i], sizeof(int) * n);
        }

        recv_all(client_socket, type, sizeof(type));
        printf("\nMatrix Type : %s\n", type);

        printf("\nDo you want to continue? (y/n): ");
        scanf(" %c", &choice);

    } while (choice == 'y' || choice == 'Y');

    n = 0;
    send_all(client_socket, &n, sizeof(n));
    close(client_socket);

    return 0;
}
