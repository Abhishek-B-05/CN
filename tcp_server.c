#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

int main()
{
    int server_socket, client_socket;
    int n, i, j;
    int matrix[50][50];
    char type[50];

    server_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = INADDR_ANY;

    bind(server_socket,
         (struct sockaddr *)&server_address,
         sizeof(server_address));

    listen(server_socket, 5);

    printf("Server waiting for client......\n");

    client_socket = accept(server_socket, NULL, NULL);

    while(1)
    {
        recv(client_socket, &n, sizeof(n), 0);

        if(n == 0)
            break;

        recv(client_socket, matrix, sizeof(int) * n * n, 0);

        printf("\nReceived Matrix:\n");

        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }

        int upper = 1;
        int lower = 1;
        int diagonal = 1;

        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                if(i > j && matrix[i][j] != 0)
                    upper = 0;

                if(i < j && matrix[i][j] != 0)
                    lower = 0;

                if(i != j && matrix[i][j] != 0)
                    diagonal = 0;
            }
        }

        if(diagonal == 1)
            strcpy(type, "Diagonal Matrix");
        else if(upper == 1)
            strcpy(type, "Upper Triangular Matrix");
        else if(lower == 1)
            strcpy(type, "Lower Triangular Matrix");
        else
            strcpy(type, "Not a Triangular Diagonal Matrix");

        send(client_socket, type, sizeof(type), 0);
    }

    close(client_socket);
    close(server_socket);

    return 0;
}
