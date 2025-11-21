#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <pthread.h>

#define CHUNK_SIZE 128

void * thread_process_chunk(void *arg)
{
    /* unpack the packet */
    void **packet = (void **) arg;

    unsigned char *start = (unsigned char *) *(packet + 0);
    int *plen = (int *) *(packet + 1);
    int length = *plen;

    free(plen);     /* free the dynamically allocated int */
    free(packet);   /* free the packet array itself */

    printf("THREAD %lu: processing my %d bytes\n",
           (unsigned long) pthread_self(), CHUNK_SIZE);

    unsigned char *ptr = start;
    unsigned char *end = start + length;

    while (ptr < end)
    {
        unsigned char c = *ptr;

        if (c == '\n') {
            *ptr = '$';
        }
        else if (isspace(c) && c != '\n') {
            *ptr = '.';
        }
        else if (islower(c)) {
            *ptr = (unsigned char) toupper(c);
        }

        ptr++;
    }

    return NULL;
}

int main(int argc, char **argv)
{
    if (argc != 3)
    {
        fprintf(stderr, "USAGE: %s <filename> <num-bytes>\n", *(argv + 0));
        return EXIT_FAILURE;
    }

    char *filename = *(argv + 1);
    long req = atol(*(argv + 2));
    if (req <= 0)
    {
        fprintf(stderr, "ERROR: number of bytes must be positive\n");
        return EXIT_FAILURE;
    }

    unsigned char *buffer = malloc((size_t) req);
    if (!buffer)
    {
        fprintf(stderr, "ERROR: malloc failed\n");
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(filename, "rb");
    if (!fp)
    {
        fprintf(stderr, "ERROR: fopen failed for '%s'\n", filename);
        free(buffer);
        return EXIT_FAILURE;
    }

    size_t bytes_read = fread(buffer, 1, (size_t) req, fp);
    fclose(fp);

    printf("MAIN: initial file contents:\n");
    unsigned char *p = buffer;
    unsigned char *end = buffer + bytes_read;
    while (p < end) putchar(*p++);
    putchar('\n');

    /* compute number of threads */
    int children = (bytes_read + CHUNK_SIZE - 1) / CHUNK_SIZE;

    printf("MAIN: creating %d child threads\n", children);

    pthread_t *tids = calloc(children, sizeof(pthread_t));
    if (!tids)
    {
        fprintf(stderr, "ERROR: calloc failed\n");
        free(buffer);
        return EXIT_FAILURE;
    }

    int i;
    for (i = 0; i < children; i++)
    {
        unsigned char *chunk_start = buffer + (i * CHUNK_SIZE);

        int chunk_len;
        if ((size_t)((i + 1) * CHUNK_SIZE) <= bytes_read)

            chunk_len = CHUNK_SIZE;
        else
            chunk_len = bytes_read - i * CHUNK_SIZE;

        /* dynamically allocate packet: 2 pointers */
        void **packet = malloc(2 * sizeof(void *));
        if (!packet)
        {
            fprintf(stderr, "ERROR: malloc failed\n");
            free(tids);
            free(buffer);
            return EXIT_FAILURE;
        }

        /* dynamic int for length (required) */
        int *plen = malloc(sizeof(int));
        if (!plen)
        {
            fprintf(stderr, "ERROR: malloc failed\n");
            free(packet);
            free(tids);
            free(buffer);
            return EXIT_FAILURE;
        }

        *plen = chunk_len;

        *(packet + 0) = chunk_start;
        *(packet + 1) = plen;

        int rc = pthread_create(tids + i, NULL, thread_process_chunk, packet);
        if (rc != 0)
        {
            fprintf(stderr, "ERROR: pthread_create failed (%d)\n", rc);
            free(plen);
            free(packet);
            free(tids);
            free(buffer);
            return EXIT_FAILURE;
        }
    }

    for (i = 0; i < children; i++)
    {
        pthread_join(*(tids + i), NULL);
        printf("MAIN: joined a child thread\n");
    }

    printf("MAIN: final file contents:\n");
    p = buffer;
    end = buffer + bytes_read;
    while (p < end) putchar(*p++);
    putchar('\n');

    free(tids);
    free(buffer);

    return EXIT_SUCCESS;
}
