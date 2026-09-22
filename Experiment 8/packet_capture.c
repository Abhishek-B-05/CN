#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>
#include <netinet/if_ether.h>

#define BUF_SIZE 65536

int main() {
    int sock;
    unsigned char buffer[BUF_SIZE];

    sock = socket(PF_PACKET, SOCK_RAW, htons(ETH_P_ALL));

    if (sock < 0) {
        perror("Socket creation failed");
        exit(1);
    }

    printf("Packet Capturing Started...\n");

    while (1) {
        int data_size = recvfrom(sock, buffer, BUF_SIZE, 0, NULL, NULL);

        if (data_size < 0) {
            perror("Recvfrom failed");
            break;
        }

        struct ethhdr *eth = (struct ethhdr *)buffer;

        if (ntohs(eth->h_proto) == ETH_P_IP) {
            struct iphdr *ip = (struct iphdr *)(buffer + sizeof(struct ethhdr));

            struct in_addr src, dst;
            src.s_addr = ip->saddr;
            dst.s_addr = ip->daddr;

            printf("Source IP: %s\n", inet_ntoa(src));
            printf("Destination IP: %s\n", inet_ntoa(dst));
            printf("Protocol: %d\n", ip->protocol);
            printf("-----------------------------\n");
        }
    }

    close(sock);
    return 0;
}
