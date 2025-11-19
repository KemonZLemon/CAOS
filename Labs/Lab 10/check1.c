#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void * threadcountdown( void * arg );

int main( int argc, char ** argv )
{
    srand( time( NULL ) * getpid() );

    if (argc != 2 && argc != 3)
    {
        fprintf(stderr, "ERROR: invalid arguments\n");
        fprintf(stderr, "USAGE: %s <#-of-child-threads> [message]\n", *(argv + 0));
        return EXIT_FAILURE;
    }

    int children = atoi(*(argv + 1));

    char *message;
    if (argc == 3)
        message = *(argv + 2);
    else
        message = "DEFAULT MESSAGE";

    pthread_t *tids = calloc(children, sizeof(pthread_t));
    if (!tids)
    {
        fprintf(stderr, "ERROR: calloc failed\n");
        return EXIT_FAILURE;
    }

    int i;
    for (i = 0; i < children; i++)
    {
        int *t = malloc(sizeof(int));
        if (!t)
        {
            fprintf(stderr, "ERROR: malloc failed\n");
            return EXIT_FAILURE;
        }

        *t = 10 + (rand() % 21);   /* pseudo-random target in [10,30] */

        printf("MAIN: creating child thread with %d...\n", *t);

        /* package data: pointer to t and pointer to message */
        void **packet = malloc(2 * sizeof(void *));
        if (!packet)
        {
            fprintf(stderr, "ERROR: malloc failed\n");
            free(t);
            return EXIT_FAILURE;
        }

        *(packet + 0) = t;         /* int* */
        *(packet + 1) = message;   /* char* */

        int rc = pthread_create(tids + i, NULL, threadcountdown, packet);
        if (rc != 0)
            fprintf(stderr, "MAIN: ERROR: pthread_create failed (%d)\n", rc);
    }

    for (i = 0; i < children; i++)
    {
        pthread_join(*(tids + i), NULL);
        printf("MAIN: joined a child thread\n");
    }

    printf("MAIN: bye -- entire process shuts down here!\n");

    free(tids);
    return EXIT_SUCCESS;
}


void * threadcountdown(void * arg)
{
    void **packet = (void **)arg;

    int *t_ptr = *(packet + 0);
    char *message = *(packet + 1);

    int target = *t_ptr;

    free(t_ptr);
    free(packet);

    printf("THREAD %lu: Counting down from 32...\n", pthread_self());

    int current = 32;
    while (current >= target)
    {
        printf("THREAD %lu: %d\n", pthread_self(), current);
        if (current == target)
        {
            printf("THREAD %lu: %d -- %s\n", pthread_self(), current, message);
            break;
        }
        current--;
        sleep(1);
    }

    return NULL;
}
