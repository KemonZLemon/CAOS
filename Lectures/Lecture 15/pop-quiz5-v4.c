/* pop-quiz5-v4.c */

/* In this version, the child threads return a value back to the
 * main thread...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

  void * q3( void * arg )
  {
    printf( "THREAD %lu: %s\n", pthread_self(), (char *)arg );

#if 0
    /* This approach is incorrect, but review to understand why... */
    int x = 6;          /* static allocation in q3() --- x: 6 (4 bytes) */
    int * retval = &x;  /* static allocation in q3() --- retval: &x (8 bytes) */
    return retval;      /* if we return retval here, that is a pointer to
                            invalid memory (out of scope) on the stack... */
#endif

    /* to return something of type "void *" we need to dynamically
     *  allocate whatever memory we need... (why? see above)
     */
    int * retval = malloc( sizeof( int ) );
    *retval = 6;
    return retval;  /* child thread terminates and returns a pointer here... */
#if 0
    return NULL;  /* child thread terminates */
#endif
  }

  int main()
  {
    printf( "hello\n" );
    char * s = "world";
    pthread_t tid1, tid2;

    pthread_create( &tid1, NULL, q3, s );  /* create first child thread */
    pthread_create( &tid2, NULL, q3, s + 2 );  /* create second child thread */

    int * retval;     /* the (void **) below will "cast" the &retval to the */
                      /* expected type for the second arg to pthread_join() */
    pthread_join( tid1, (void **)&retval );  /* join the first child thread */
    printf( "MAIN: joined a thread that returned %d\n", *retval );
    free( retval );   /* free the memory that the child thread allocated */

    pthread_join( tid2, (void **)&retval );  /* join the second child thread */
    printf( "MAIN: joined a thread that returned %d\n", *retval );
    free( retval );   /* free the memory that the child thread allocated */

    printf( "bye\n" );
    return EXIT_SUCCESS;  /* main thread terminates */
  }

