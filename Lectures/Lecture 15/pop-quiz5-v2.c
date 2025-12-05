/* pop-quiz5-v2.c */

/* In this version, we are guaranteed to see all four lines of output */
/* ...because we are joining the child threads back in with the main thread */

/* There is exactly ONE and only one possible output for this version... */
/* ...because of where the pthread_join() calls are made */

/* (as a result, the threads are not running in parallel...) */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

  void * q3( void * arg )
  {
#if 0
    pthread_detach( pthread_self() );
#endif
    printf( "%s\n", (char *)arg );
    return NULL;  /* child thread terminates */
  }

  int main()
  {
    printf( "hello\n" );
    char * s = "world";
    pthread_t tid;
    pthread_create( &tid, NULL, q3, s );  /* create first child thread */
    pthread_join( tid, NULL );  /* join the first child thread */
    pthread_create( &tid, NULL, q3, s );  /* create second child thread */
    pthread_join( tid, NULL );  /* join the second child thread */
    printf( "bye\n" );
    return EXIT_SUCCESS;  /* main thread terminates */
  }
