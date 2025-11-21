#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include <string.h>

void * threadcountdown(void * arg);

int main(int argc, char **argv)
{
    srand(time(NULL) * getpid());  // Seed random numbers 

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <#-of-child-threads> [message]\n", *(argv + 0));
        return EXIT_FAILURE;
    }

    int children = atoi(*(argv + 1));  // Number of threads to create
    char * message = (argc >= 3) ? *(argv + 2) : "default message";

    pthread_t *tids = calloc(children, sizeof(pthread_t));  // Thread IDs

    for (int i = 0; i < children; i++) {
        int *t = malloc(sizeof(int));  // Each thread gets its own t value
        *t = 10 + (rand() % 21);       // Random stop value between 10-30
        printf("MAIN: creating child thread with %d...\n", *t);
        pthread_create(tids + i, NULL, threadcountdown, (void *)t);
    }

    for (int i = 0; i < children; i++) {
        pthread_join(*(tids + i), NULL);  // Wait for each thread to finish
        printf("MAIN: joined a child thread\n");
    }

    printf("MAIN: bye -- entire process shuts down here!\n");
    free(tids);  // Free allocated thread array
    return EXIT_SUCCESS;
}

void * threadcountdown(void * arg)
{
    int t = *(int *)arg;
    free(arg);  // Free memory allocated for t

    printf("THREAD %lu: Counting down from 32...\n", pthread_self());

    for (int i = 32; i >= t; i--) {
        printf("THREAD %lu: %d\n", pthread_self(), i);
        sleep(1);  // Wait 1 second per count
    }

    printf("THREAD %lu: %d -- done!\n", pthread_self(), t);
    return NULL;
}
