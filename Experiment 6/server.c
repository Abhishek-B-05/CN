#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <ctype.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define PORT 8080
#define BUFFER_SIZE 1024

// Utility function to convert string to uppercase
void to_uppercase(char *str) {
    for (int i = 0; str[i]; i++) {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main() {
    int sockfd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // 1. Create UDP socket
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // 2. Bind socket to port
    if (bind(sockfd, (const struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Bind failed");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    printf("UDP Time Server running on port %d...\n", PORT);

    // Main infinite loop: Continuously process requests from any client concurrently
    while (1) {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                                     (struct sockaddr *)&client_addr, &addr_len);
        
        if (bytes_received < 0) continue;

        // Clean trailing newlines/spaces
        buffer[strcspn(buffer, "\r\n")] = 0;

        // Fork a child process to handle this specific request concurrently
        pid_t pid = fork();

        if (pid == 0) { 
            // --- Child Process ---
            char clean_req[BUFFER_SIZE];
            strcpy(clean_req, buffer);
            to_uppercase(clean_req);

            // Respond ONLY if request is exactly "TIME"
            if (strcmp(clean_req, "TIME") == 0) {
                time_t rawtime;
                struct tm *timeinfo;
                time(&rawtime);
                timeinfo = localtime(&rawtime);

                char time_str[BUFFER_SIZE];
                strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", timeinfo);

                sendto(sockfd, time_str, strlen(time_str), 0,
                       (struct sockaddr *)&client_addr, addr_len);
                printf("[LOG] Sent time to %s:%d\n", 
                       inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            } else {
                printf("[LOG] Ignored command ('%s') from %s:%d\n", 
                       buffer, inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            }
            
            close(sockfd);
            exit(0); // Terminate child process after handling one request
        }
    }

    close(sockfd);
    return 0;
}
