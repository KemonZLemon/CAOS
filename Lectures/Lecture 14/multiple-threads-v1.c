/* multiple-threads-v1.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

void * whattodo( void * arg );

int main( int argc, char ** argv )
{
  srand( time( NULL ) * getpid() );

  if ( argc != 2 )
  {
    fprintf( stderr, "ERROR: invalid arguments\n" );
    fprintf( stderr, "USAGE: %s <#-of-child-threads>\n", *(argv+0) );
    return EXIT_FAILURE;
  }

  int children = atoi( *(argv + 1) );

  pthread_t * tids = calloc( children, sizeof( pthread_t ) );

  int i, rc;

  /* create the child threads (in the main thread) */
  for ( i = 0 ; i < children; i++ )
  {
    /* dynamically allocate an int to pass into the thread code/function...
     * ...this will ensure that each thread has
     *  its own dedicated separate t variable to work with
     */
    int * t = malloc( sizeof( int ) );
    *t = 10 + ( rand() % 21 );   /* [10,30] seconds */
    printf( "MAIN: creating child thread with %d...\n", *t );
    rc = pthread_create( tids + i, NULL, whattodo, t );
    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: ERROR: pthread_create() failed (%d)\n", rc );
    }
  }

  /* join all child threads, i.e., acknowledge each child thread's termination */
  for ( i = 0 ; i < children ; i++ )
  {
    pthread_join( *(tids + i), NULL );    /* BLOCKING CALL */
    printf( "MAIN: joined a child thread\n" );
  }

  printf( "MAIN: bye -- entire process shuts down here!\n" );

  free( tids );
  return EXIT_SUCCESS;
}

void * whattodo( void * arg )
{
  int t = *(int *)arg;   /* (int *)arg will "cast" arg as a pointer to int */
  free( arg );

#if 0
  /* This child thread is set up to *not* be joined by its parent thread */
  pthread_detach( pthread_self() );
#endif

  printf( "THREAD %lu: Counting down from %d...\n", pthread_self(), t );
  while ( t > 0 )
  {
    printf( "THREAD %lu: %d\n", pthread_self(), t-- );
    sleep( 1 );
  }
  printf( "THREAD %lu: done!\n", pthread_self() );

  return NULL;
}



