#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>

#define CHUNK_SIZE 32

struct packet {
    unsigned char *start;
    size_t length;
};

void * thread_process_chunk( void * arg )
{
    struct packet *p = (struct packet *) arg;
    if (p == NULL)
        return NULL;

    unsigned char *ptr = p->start;
    size_t len = p->length;

    printf( "THREAD %lu: processing my %d bytes\n", (unsigned long)pthread_self(), CHUNK_SIZE );

    unsigned char *end = ptr + len;
    while (ptr < end)
    {
        unsigned char c = *ptr;

        if (c == '\n') {
            *ptr = '$';
        }
        else if (isspace(c) && c != '\n') {
            /* covers space, tab, etc. (newline already handled) */
            *ptr = '.';
        }
        else if (islower(c)) {
            *ptr = (unsigned char) toupper(c);
        }
        /* other characters unchanged */

        ptr++;
    }

    free(p);
    return NULL;
}

int main( int argc, char ** argv )
{
    if (argc != 3)
    {
        fprintf( stderr, "USAGE: %s <filename> <num-bytes>\n", *(argv + 0) );
        return EXIT_FAILURE;
    }

    char *filename = *(argv + 1);
    long requested = atol( *(argv + 2) );
    if (requested <= 0)
    {
        fprintf( stderr, "ERROR: <num-bytes> must be positive\n" );
        return EXIT_FAILURE;
    }

    /* allocate buffer for file contents */
    unsigned char *buffer = malloc( (size_t) requested );
    if (buffer == NULL)
    {
        fprintf( stderr, "ERROR: malloc failed\n" );
        return EXIT_FAILURE;
    }

    /* open and read file */
    FILE *fp = fopen( filename, "rb" );
    if (fp == NULL)
    {
        fprintf( stderr, "ERROR: fopen failed for '%s'\n", filename );
        free( buffer );
        return EXIT_FAILURE;
    }

    size_t bytes_read = fread( buffer, 1, (size_t) requested, fp );
    fclose( fp );

    /* print initial file contents */
    printf( "MAIN: initial file contents:\n" );
    if (bytes_read > 0)
    {
        unsigned char *p = buffer;
        unsigned char *end = buffer + bytes_read;
        while (p < end)
        {
            putchar( *p );
            p++;
        }
        if ( (end == buffer) || (*(end - 1) != '\n') )
            putchar( '\n' ); /* ensure newline after display for neatness */
    }
    else
    {
        printf( "(file empty or nothing read)\n" );
    }

    /* compute number of child threads (ceiling division) */
    size_t children = (bytes_read + CHUNK_SIZE - 1) / CHUNK_SIZE;
    if (children == 0)
        children = 0; /* no threads needed */

    printf( "MAIN: creating %zu child threads\n", children );

    pthread_t *tids = calloc( children, sizeof( pthread_t ) );
    if (tids == NULL && children > 0)
    {
        fprintf( stderr, "ERROR: calloc failed\n" );
        free( buffer );
        return EXIT_FAILURE;
    }

    size_t i;
    for (i = 0; i < children; i++)
    {
        /* compute chunk start and length using pointer arithmetic */
        unsigned char *chunk_start = buffer + (i * CHUNK_SIZE);

        size_t chunk_len;
        if ( (i + 1) * CHUNK_SIZE <= bytes_read )
            chunk_len = CHUNK_SIZE;
        else
            chunk_len = bytes_read - (i * CHUNK_SIZE);

        struct packet *p = malloc( sizeof( struct packet ) );
        if (p == NULL)
        {
            fprintf( stderr, "ERROR: malloc failed for packet\n" );
            /* clean up and exit */
            size_t j;
            for (j = 0; j < i; j++)
                pthread_join( *(tids + j), NULL );
            free( tids );
            free( buffer );
            return EXIT_FAILURE;
        }

        p->start = chunk_start;
        p->length = chunk_len;

        int rc = pthread_create( tids + i, NULL, thread_process_chunk, p );
        if (rc != 0)
        {
            fprintf( stderr, "MAIN: ERROR: pthread_create failed (%d)\n", rc );
            free(p);
            /* join previously created threads, then cleanup */
            size_t j;
            for (j = 0; j < i; j++)
                pthread_join( *(tids + j), NULL );
            free( tids );
            free( buffer );
            return EXIT_FAILURE;
        }
    }

    /* join all child threads */
    for (i = 0; i < children; i++)
    {
        pthread_join( *(tids + i), NULL );
        printf( "MAIN: joined a child thread\n" );
    }

    /* print final file contents */
    printf( "MAIN: final file contents:\n" );
    if (bytes_read > 0)
    {
        unsigned char *p = buffer;
        unsigned char *end = buffer + bytes_read;
        while (p < end)
        {
            putchar( *p );
            p++;
        }
        putchar( '\n' );
    }
    else
    {
        printf( "(file empty or nothing read)\n" );
    }

    free( tids );
    free( buffer );

    return EXIT_SUCCESS;
}
