#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define SIZE 1024

void replace_word(char *sentence, const char *word, const char *replacement)
{
    char buffer[SIZE];
    char *pos;
    int word_len = strlen(word);

    while ((pos = strstr(sentence, word)) != NULL)
    {
        buffer[0] = '\0';

        strncat(buffer, sentence, pos - sentence);
        strcat(buffer, replacement);
        strcat(buffer, pos + word_len);

        strcpy(sentence, buffer);
    }
}

int main()
{
    int sockfd;
    char sentence[SIZE];

    struct sockaddr_in server, client;
    socklen_t client_len = sizeof(client);

    // Create UDP socket
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    // Server address
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    // Bind socket
    bind(sockfd, (struct sockaddr *)&server, sizeof(server));

    printf("UDP Server waiting...\n");

    // Receive sentence
    recvfrom(sockfd, sentence, SIZE, 0,
             (struct sockaddr *)&client, &client_len);

    printf("Received: %s\n", sentence);

    // Translate abbreviations
    replace_word(sentence, "tbh", "to be honest");
    replace_word(sentence, "ig", "I guess");
    replace_word(sentence, "tbf", "to be fair");
    replace_word(sentence, "atm", "at the moment");
    replace_word(sentence, "irl", "in real life");
    replace_word(sentence, "lol", "laughing out loud");
    replace_word(sentence, "asap", "as soon as possible");
    replace_word(sentence, "omg", "oh my god");
    replace_word(sentence, "tyl", "talk to you later");
    replace_word(sentence, "idk", "I don't know");
    replace_word(sentence, "nvm", "never mind");

    printf("Translated: %s\n", sentence);

    // Send translated sentence
    sendto(sockfd, sentence, strlen(sentence) + 1, 0,
           (struct sockaddr *)&client, client_len);

    close(sockfd);

    return 0;
}