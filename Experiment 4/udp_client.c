#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SIZE 1024

int main()
{
    int sockfd;
    char sentence[SIZE];

    struct sockaddr_in server;
    socklen_t server_len = sizeof(server);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Server address
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    server.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Input sentence
    printf("Enter new-generation English sentence:\n");
    fgets(sentence, SIZE, stdin);

    // Remove newline
    sentence[strcspn(sentence, "\n")] = '\0';

    // Send sentence to server
    sendto(sockfd, sentence, strlen(sentence) + 1, 0,
           (struct sockaddr *)&server, server_len);

    // Receive translated sentence
    recvfrom(sockfd, sentence, SIZE, 0,
             (struct sockaddr *)&server, &server_len);

    printf("\nFormal English:\n%s\n", sentence);

    close(sockfd);

    return 0;
}