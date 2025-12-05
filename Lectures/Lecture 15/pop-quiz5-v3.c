/* pop-quiz5-v3.c */

/* NOTE: for each child thread created, you MUST call either
 *        pthread_detach() in the child thread code
 *        or pthread_join() in the parent (main) thread code
 *
 * Also note that pthread_join() is ALWAYS a blocking call
 */

/* In this version, we are guaranteed to see all four lines of output */
/* ...because we are joining the child threads back in with the main thread */
/* (but this time, child threads are running in parallel) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

  void * q3( void * arg )
  {
#if 0
    pthread_detach( pthread_self() );
#endif
    printf( "THREAD %lu: %s\n", pthread_self(), (char *)arg );
    return NULL;  /* child thread terminates */
  }

  int main()
  {
    printf( "hello\n" );
    char * s = "world";
    pthread_t tid1, tid2;

    pthread_create( &tid1, NULL, q3, s );  /* create first child thread */
    pthread_create( &tid2, NULL, q3, s + 2 );  /* create second child thread */
#if 0
    printf( "MAIN: ...\n" );    /* this would also be interleaved... */
    /* TO DO: revise the "Two possible outputs" picture in the comment below */
#endif
    pthread_join( tid1, NULL );  /* join the first child thread */
    pthread_join( tid2, NULL );  /* join the second child thread */

    printf( "bye\n" );
    return EXIT_SUCCESS;  /* main thread terminates */
  }

/* Two possible outputs:
 *
 * hello
 * THREAD 133820741318336: world
 * THREAD 133820732925632: rld
 * bye
 *
 * hello
 * THREAD 133820732925632: rld    <== unlikely, but possible
 * THREAD 133820741318336: world
 * bye
 *
 */
