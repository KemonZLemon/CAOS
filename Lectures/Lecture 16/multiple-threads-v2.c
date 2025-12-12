/* multiple-threads-v2.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#define RANGE 5

/* Add 0 + 1 + 2 + 3 + ... + max in parallel using child threads */
int max;

/* The child threads will all add to this shared global variable;
 *  once all threads complete, the main thread will display the grand total.
 */
int total = 0;

/* thread function (see below) */
void * sum( void * arg );

int main( int argc, char ** argv )
{
  if ( argc != 2 )
  {
    fprintf( stderr, "ERROR: invalid arguments\n" );
    fprintf( stderr, "USAGE: %s <max>\n", *(argv+0) );
    return EXIT_FAILURE;
  }

  max = atoi( *(argv + 1) );  /* TO DO: validate input */
  int children = max / RANGE + 1;   /* TO DO: does this always work? */
  pthread_t * tids = calloc( children, sizeof( pthread_t ) );

  int i, rc;

  /* create the child threads (in the main thread) */
  for ( i = 0 ; i < children ; i++ )
  {
    /* dynamically allocate an int to pass into the thread code/function...
     * ...this will ensure that each thread has
     *  its own dedicated separate int variable to work with
     */
    int * start = malloc( sizeof( int ) );
    *start = i * RANGE;
    int end = *start + RANGE - 1;
    if ( end > max ) end = max;
    printf( "MAIN: creating child thread to add %d-%d to total\n", *start, end );

    rc = pthread_create( tids + i, NULL, sum, start );

    if ( rc != 0 )
    {
      fprintf( stderr, "MAIN: ERROR: pthread_create() failed (%d)\n", rc );
    }
  }

  /* join all child threads, i.e., acknowledge each child thread's termination */
  for ( i = 0 ; i < children ; i++ )
  {
    pthread_join( *(tids + i), NULL );    /* BLOCKING CALL */
/*    printf( "MAIN: joined a child thread\n" ); */
  }

  printf( "MAIN: all threads successfully joined\n" );

  int expected = 0;
  for ( int i = 1 ; i <= max ; i++ ) expected += i;
  printf( "MAIN: total is %d -- expected sum is %d\n", total, expected );

  free( tids );
  return EXIT_SUCCESS;
}

void * sum( void * arg )
{
  int start = *(int *)arg;  /* (int *)arg will "cast" arg as a pointer to int */
  free( arg );

  int end = start + RANGE - 1;
  if ( end > max ) end = max;

  for ( int i = start ; i <= end ; i++ )
  {
    printf( "THREAD %lu: Adding %d to global total...\n", pthread_self(), i );
    total += i;  /* add i to the global total variable */
  }

  return NULL;
}





