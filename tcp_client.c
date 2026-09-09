#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <time.h>

int main()
{
    int client_socket;
    int n, i, j;
    int matrix[50][50];
    char type[50];
    int choice;

    client_socket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_address;

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(8080);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    connect(client_socket,
            (struct sockaddr *)&server_address,
            sizeof(server_address));

    srand(time(0));

    while(1)
    {
        printf("\nEnter N (0 to exit): ");
        scanf("%d", &n);

        send(client_socket, &n, sizeof(n), 0);

        if(n == 0)
            break;

        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                matrix[i][j] = rand() % 50 + 1;
            }
        }

        printf("\nGenerated Matrix:\n");

        for(i = 0; i < n; i++)
        {
            for(j = 0; j < n; j++)
            {
                printf("%d ", matrix[i][j]);
            }
            printf("\n");
        }

        send(client_socket,
             matrix,
             sizeof(int) * n * n,
             0);

        recv(client_socket,
             type,
             sizeof(type),
             0);

        printf("\nMatrix Type : %s\n", type);
    }

    close(client_socket);

    return 0;
}
