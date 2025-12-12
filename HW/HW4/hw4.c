
#define _POSIX_C_SOURCE 200112L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define MAX_UDP 128

typedef struct client_msg {
    int sockfd;
    struct sockaddr_in addr;
    socklen_t addrlen;
    unsigned int len;
    char *data;
} client_msg_t;

/* print error and exit thread */
static void thread_perror_exit(const char *msg) {
    if (msg) {
        fprintf(stderr, "ERROR: %s\n", msg);
    } else {
        perror("ERROR");
    }
    pthread_exit(NULL);
}

/* SIGUSR1 handler */
static void sigusr1_handler(int s) {
    (void)s;
    puts("MAIN: SIGUSR1 received; shutting down the server...");
    fflush(stdout);
    exit(EXIT_SUCCESS);
}

/* append single char to dynamic buffer; uses realloc */
static int append_char(char **bufp, unsigned int *cap, unsigned int *lenp, char c) {
    if (*lenp + 1u >= *cap) {
        unsigned int newcap = (*cap == 0u) ? 64u : (*cap * 2u);
        char *tmp = realloc(*bufp, newcap);
        if (!tmp) return -1;
        *bufp = tmp;
        *cap = newcap;
    }
    *( *bufp + *lenp ) = c;
    *lenp += 1u;
    return 0;
}

/* append null-terminated C string (source pointer p) */
static int append_cstr(char **bufp, unsigned int *cap, unsigned int *lenp, const char *p) {
    while (*p) {
        if (append_char(bufp, cap, lenp, *p) != 0) return -1;
        p++;
    }
    return 0;
}

/* encode payload: process p of length n, return calloc'd buffer and set out_len */
static char *encode_payload(const char *p, unsigned int n, unsigned int *out_len) {
    unsigned int cap = 0;
    unsigned int outl = 0;
    char *out = NULL;
    const char *endp = p + n;
    while (p < endp) {
        unsigned char uc = (unsigned char)(*p);
        if (uc == '\n') {
            if (append_cstr(&out, &cap, &outl, "%%%%%") != 0) goto fail;
        } else if (uc == ' ') {
            if (append_char(&out, &cap, &outl, '.') != 0) goto fail;
        } else if (uc == '\t') {
            if (append_cstr(&out, &cap, &outl, "..|..") != 0) goto fail;
        } else if (isdigit(uc)) {
            if (append_char(&out, &cap, &outl, (char)uc) != 0) goto fail;
            if (append_char(&out, &cap, &outl, '#') != 0) goto fail;
            if (append_char(&out, &cap, &outl, (char)uc) != 0) goto fail;
        } else if (isalpha(uc)) {
            char base = (('A' <= uc && uc <= 'Z') ? 'A' : 'a');
            int offset = (int)(uc - base);
            int mapped = (offset + 3) % 26;
            char rc = (char)(base + mapped);
            if (append_char(&out, &cap, &outl, rc) != 0) goto fail;
        } else {
            if (append_char(&out, &cap, &outl, (char)uc) != 0) goto fail;
        }
        p++;
    }

    /* add terminating null for safety */
    if (append_char(&out, &cap, &outl, '\0') != 0) goto fail;
    if (outl > 0u) outl--;
    *out_len = outl;
    return out;

fail:
    if (out) free(out);
    *out_len = 0u;
    return NULL;
}

/* decode payload: inverse transform; process left-to-right */
static char *decode_payload(const char *p, unsigned int n, unsigned int *out_len) {
    unsigned int cap = 0u;
    unsigned int outl = 0u;
    char *out = NULL;
    const char *endp = p + n;
    while (p < endp) {
        unsigned char uc = (unsigned char)(*p);

        /* check for "%%%%%" sequence */
        if (uc == '%') {
            if ((unsigned int)(endp - p) >= 5u &&
                *(p + 0u) == '%' && *(p + 1u) == '%' && *(p + 2u) == '%' && *(p + 3u) == '%' && *(p + 4u) == '%') {
                if (append_char(&out, &cap, &outl, '\n') != 0) goto fail;
                p += 5u;
                continue;
            }
            if (append_char(&out, &cap, &outl, '%') != 0) goto fail;
            p++;
            continue;
        }

        /* check for "..|.." tab */
        if (uc == '.') {
            if ((unsigned int)(endp - p) >= 5u &&
                *(p + 0u) == '.' && *(p + 1u) == '.' && *(p + 2u) == '|' && *(p + 3u) == '.' && *(p + 4u) == '.') {
                if (append_char(&out, &cap, &outl, '\t') != 0) goto fail;
                p += 5u;
                continue;
            }
            /* single dot -> space */
            if (append_char(&out, &cap, &outl, ' ') != 0) goto fail;
            p++;
            continue;
        }

        /* check for digit pattern d#d */
        if (isdigit(uc)) {
            if ((unsigned int)(endp - p) >= 3u &&
                isdigit(*(p + 0u)) && (*(p + 1u) == '#') && (*(p + 2u) == *(p + 0u))) {
                if (append_char(&out, &cap, &outl, *(p + 0u)) != 0) goto fail;
                p += 3u;
                continue;
            }
            if (append_char(&out, &cap, &outl, (char)uc) != 0) goto fail;
            p++;
            continue;
        }

        /* letters shift back by 3 */
        if (isalpha(uc)) {
            char base = (('A' <= uc && uc <= 'Z') ? 'A' : 'a');
            int offset = (int)(uc - base);
            int mapped = (offset - 3) % 26;
            if (mapped < 0) mapped += 26;
            char rc = (char)(base + mapped);
            if (append_char(&out, &cap, &outl, rc) != 0) goto fail;
            p++;
            continue;
        }

        /* default copy */
        if (append_char(&out, &cap, &outl, (char)uc) != 0) goto fail;
        p++;
    }

    if (append_char(&out, &cap, &outl, '\0') != 0) goto fail;
    if (outl > 0u) outl--;
    *out_len = outl;
    return out;

fail:
    if (out) free(out);
    *out_len = 0u;
    return NULL;
}

/* thread to handle encode */
static void *encode_message(void *arg) {
    client_msg_t *cm = (client_msg_t *)arg;
    if (!cm) pthread_exit(NULL);
    if (cm->len < 1u) {
        free(cm->data);
        free(cm);
        pthread_exit(NULL);
    }

    unsigned int data_len = cm->len - 1u;
    const char *payload = cm->data + 1u;

    printf("THREAD: Received encode request (%u bytes)\n", data_len);
    fflush(stdout);

    unsigned int out_len = 0u;
    char *encoded = encode_payload(payload, data_len, &out_len);
    if (!encoded) {
        fprintf(stderr, "ERROR: encoding failed\n");
        free(cm->data);
        free(cm);
        pthread_exit(NULL);
    }

    unsigned int resp_total = out_len + 1u;
    char *resp = calloc(resp_total, 1u);
    if (!resp) {
        free(encoded);
        free(cm->data);
        free(cm);
        thread_perror_exit("calloc failed for response");
    }

    *(resp + 0u) = *(cm->data + 0u);
    unsigned int i = 0u;
    while (i < out_len) {
        *(resp + 1u + i) = *(encoded + i);
        i++;
    }

    ssize_t sent = sendto(cm->sockfd, resp, resp_total, 0,
                          (struct sockaddr *)&cm->addr, cm->addrlen);
    if (sent < 0) {
        perror("ERROR");
    } else {
        printf("THREAD: Sent encoded response (%u bytes)\n", out_len);
        fflush(stdout);
    }

    free(resp);
    free(encoded);
    free(cm->data);
    free(cm);
    pthread_exit(NULL);
}

/* thread to handle decode */
static void *decode_message(void *arg) {
    client_msg_t *cm = (client_msg_t *)arg;
    if (!cm) pthread_exit(NULL);
    if (cm->len < 1u) {
        free(cm->data);
        free(cm);
        pthread_exit(NULL);
    }

    unsigned int data_len = cm->len - 1u;
    const char *payload = cm->data + 1u;

    printf("THREAD: Received decode request (%u bytes)\n", data_len);
    fflush(stdout);

    unsigned int out_len = 0u;
    char *decoded = decode_payload(payload, data_len, &out_len);
    if (!decoded) {
        fprintf(stderr, "ERROR: decoding failed\n");
        free(cm->data);
        free(cm);
        pthread_exit(NULL);
    }

    unsigned int resp_total = out_len + 1u;
    char *resp = calloc(resp_total, 1u);
    if (!resp) {
        free(decoded);
        free(cm->data);
        free(cm);
        thread_perror_exit("calloc failed for response");
    }

    *(resp + 0u) = *(cm->data + 0u);
    unsigned int i = 0u;
    while (i < out_len) {
        *(resp + 1u + i) = *(decoded + i);
        i++;
    }

    ssize_t sent = sendto(cm->sockfd, resp, resp_total, 0,
                          (struct sockaddr *)&cm->addr, cm->addrlen);
    if (sent < 0) {
        perror("ERROR");
    } else {
        printf("THREAD: Sent decoded response (%u bytes)\n", out_len);
        fflush(stdout);
    }

    free(resp);
    free(decoded);
    free(cm->data);
    free(cm);
    pthread_exit(NULL);
}

/* parse port string pointed by s; return -1 on error */
static int parse_port(const char *s) {
    if (!s) return -1;
    const char *p = s;
    if (*p == '\0') return -1;
    while (*p) {
        if (!isdigit((unsigned char)*p)) return -1;
        p++;
    }
    long val = strtol(s, NULL, 10);
    if (val <= 0L || val > 65535L) return -1;
    return (int)val;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "ERROR: Invalid argument\nUSAGE: ./hw4.out <UDP-server-port>\n");
        return EXIT_FAILURE;
    }

    char *portstr = *(argv + 1u);
    int port = parse_port(portstr);
    if (port < 0) {
        fprintf(stderr, "ERROR: Invalid argument\nUSAGE: ./hw4.out <UDP-server-port>\n");
        return EXIT_FAILURE;
    }

    signal(SIGINT, SIG_IGN);
    signal(SIGTERM, SIG_IGN);
    signal(SIGUSR2, SIG_IGN);

    struct sigaction sa;
    sa.sa_handler = sigusr1_handler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGUSR1, &sa, NULL);

    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("ERROR");
        return EXIT_FAILURE;
    }

    struct sockaddr_in *servptr = calloc(1u, sizeof(struct sockaddr_in));
    if (!servptr) {
        close(sockfd);
        thread_perror_exit("calloc failed for server addr");
    }
    servptr->sin_family = AF_INET;
    servptr->sin_addr.s_addr = INADDR_ANY;
    servptr->sin_port = htons((unsigned short)port);

    if (bind(sockfd, (struct sockaddr *)servptr, sizeof(struct sockaddr_in)) < 0) {
        perror("ERROR");
        free(servptr);
        close(sockfd);
        return EXIT_FAILURE;
    }

    free(servptr);

    puts("MAIN: UDP server is ready to receive datagrams...");
    fflush(stdout);

    for (;;) {
        client_msg_t *cm = calloc(1u, sizeof(client_msg_t));
        if (!cm) {
            perror("ERROR");
            close(sockfd);
            return EXIT_FAILURE;
        }
        cm->sockfd = sockfd;
        cm->addrlen = sizeof(cm->addr);

        char *buf = calloc((size_t)MAX_UDP + 1u, 1u);
        if (!buf) {
            free(cm);
            perror("ERROR");
            close(sockfd);
            return EXIT_FAILURE;
        }
        cm->data = buf;

        ssize_t r = recvfrom(sockfd, buf, MAX_UDP, 0,
                             (struct sockaddr *)&cm->addr, &cm->addrlen);
        if (r < 0) {
            if (errno == EINTR) {
                free(cm->data);
                free(cm);
                continue;
            }
            perror("ERROR");
            free(cm->data);
            free(cm);
            break;
        }

        if (r > (ssize_t)MAX_UDP) r = MAX_UDP;
        cm->len = (unsigned int)r;
        if (cm->len == 0u) {
            free(cm->data);
            free(cm);
            continue;
        }

        char type = *(cm->data + 0u);
        pthread_t tid;
        int rc;
        if (type == 'E') {
            rc = pthread_create(&tid, NULL, encode_message, cm);
        } else if (type == 'D') {
            rc = pthread_create(&tid, NULL, decode_message, cm);
        } else {
            /* echo back first byte with empty payload */
            char *resp = calloc(1u, 1u);
            if (resp) {
                *(resp + 0u) = *(cm->data + 0u);
                ssize_t s = sendto(sockfd, resp, 1, 0, (struct sockaddr *)&cm->addr, cm->addrlen);
                if (s < 0) perror("ERROR");
                free(resp);
            } else {
                perror("ERROR");
            }
            free(cm->data);
            free(cm);
            continue;
        }

        if (rc != 0) {
            fprintf(stderr, "ERROR: pthread_create failed\n");
            free(cm->data);
            free(cm);
            continue;
        }

        pthread_detach(tid);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
