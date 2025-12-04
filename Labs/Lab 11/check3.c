#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

int count_lakers(char *buf, int len)
{
    char *p = buf;
    int count = 0;

    for (int i = 0; i <= len - 6; i++, p++) {
        char *q = p;
        char *pattern = "lakers";
        int matched = 1;

        for (int j = 0; j < 6; j++) {
            if (tolower(*(q + j)) != *(pattern + j)) {
                matched = 0;
                break;
            }
        }

        if (matched) count++;
    }

    return count;
}

int main(int argc, char **argv)
{
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <port> <max-datagram-size>\n", *argv);
        return EXIT_FAILURE;
    }

    int port = atoi(*(argv + 1));
    int max = atoi(*(argv + 2));
    if (max <= 0) {
        fprintf(stderr, "max-datagram-size must be > 0\n");
        return EXIT_FAILURE;
    }

    int sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd == -1) { perror("socket() failed"); return EXIT_FAILURE; }

    printf("Server-side UDP socket created on descriptor %d\n", sd);

    struct sockaddr_in server;
    int slen = sizeof(server);

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    server.sin_port = htons(port);

    if (bind(sd, (struct sockaddr *)&server, slen) == -1) {
        perror("bind() failed");
        return EXIT_FAILURE;
    }

    if (getsockname(sd, (struct sockaddr *)&server, (socklen_t *)&slen) == -1) {
        perror("getsockname() failed");
        return EXIT_FAILURE;
    }

    printf("UDP server bound to port %d\n", ntohs(server.sin_port));

    while (1)
    {
        char *buffer = (char *)malloc(max);
        if (!buffer) { perror("malloc"); continue; }

        struct sockaddr_in client;
        int clen = sizeof(client);

        int n = recvfrom(sd, buffer, max, 0,
                         (struct sockaddr *)&client,
                         (socklen_t *)&clen);

        if (n == -1) {
            perror("recvfrom() failed");
            free(buffer);
            continue;
        }

        printf("UDP DATAGRAM RECEIVED from %s port %d\n",
               inet_ntoa(client.sin_addr),
               ntohs(client.sin_port));

        printf("UDP DATAGRAM RECEIVED: \"%.*s\"\n", n, buffer);

        int lakers_count = count_lakers(buffer, n);

        // dynamically allocated reply array (6 bytes)
        unsigned char *reply = (unsigned char *)malloc(6);
        if (!reply) { perror("malloc"); free(buffer); continue; }

        unsigned short net_count = htons((unsigned short)lakers_count);
        unsigned int net_bytes = htonl((unsigned int)n);

        memcpy(reply, &net_count, 2);
        memcpy(reply + 2, &net_bytes, 4);

        int rc = sendto(sd, reply, 6, 0, (struct sockaddr *)&client, clen);
        if (rc == -1) perror("sendto() failed");

        free(buffer);
        free(reply);
    }

    close(sd);
    return EXIT_SUCCESS;
}
