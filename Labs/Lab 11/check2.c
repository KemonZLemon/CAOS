#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <ctype.h>

int count_lakers(char * buf)
{
    char * p = buf;
    int count = 0;

    while (*p != '\0') {

        char * q = p;

        char pattern[] = "LAKERS";
        char * r = pattern;

        int matched = 1;

        while (*r != '\0') {
            if (*q == '\0') { matched = 0; break; }

            char c1 = *q;
            char c2 = *r;

            if (tolower(c1) != tolower(c2)) {
                matched = 0;
                break;
            }
            q++;
            r++;
        }

        if (matched == 1)
            count++;

        p++;
    }

    return count;
}

int main(int argc, char ** argv)
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
        char * buffer = malloc(max + 1);
        if (buffer == NULL) { perror("malloc"); continue; }

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

        *(buffer + n) = '\0';

        printf("UDP DATAGRAM RECEIVED: \"%s\"\n", buffer);

        int count = count_lakers(buffer);

        char reply[32];
        snprintf(reply, sizeof(reply), "%d", count);

        int rc = sendto(sd, reply, strlen(reply), 0,
                        (struct sockaddr *)&client, clen);

        if (rc == -1)
            perror("sendto() failed");

        free(buffer);
    }

    close(sd);
    return EXIT_SUCCESS;
}
