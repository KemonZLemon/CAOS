/* pop-quiz5.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

  void * q3( void * arg )
  {
    pthread_detach( pthread_self() );
    printf( "%s\n", (char *)arg );
    return NULL;  /* child thread terminates */
  }

  int main()
  {
    printf( "hello\n" );
    char * s = "world";
    pthread_t tid;
    pthread_create( &tid, NULL, q3, s );
    pthread_create( &tid, NULL, q3, s );
    printf( "bye\n" );
    return EXIT_SUCCESS;  /* main thread terminates */
  }
