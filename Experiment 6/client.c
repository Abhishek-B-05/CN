#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1"
#define BUFFER_SIZE 1024

void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    char message[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    socklen_t addr_len = sizeof(server_addr);

    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

    struct timeval tv = {2, 0};
    setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv));

    printf("Connected to server. Type 'TIME' for system time, or 'STOP' to exit.\n\n");
    
    while (1) {
        printf("Enter command: ");
        if (fgets(message, BUFFER_SIZE, stdin) == NULL) break;

        message[strcspn(message, "\r\n")] = 0;

        char check_stop[BUFFER_SIZE];
        strcpy(check_stop, message);
        to_uppercase(check_stop);

        if (strcmp(check_stop, "STOP") == 0) {
            printf("Exiting client...\n");
            break;
        }

        sendto(sockfd, message, strlen(message), 0,
               (const struct sockaddr *)&server_addr, sizeof(server_addr));

        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                     (struct sockaddr *)&server_addr, &addr_len);

        if (bytes_received > 0) {
            buffer[bytes_received] = '\0';
            printf("Server Response: %s\n\n", buffer);
        } else {
            printf("[No response from server]\n\n");
        }
    }

    close(sockfd);
    return 0;
}
