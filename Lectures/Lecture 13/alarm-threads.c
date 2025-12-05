/* alarm-threads.c */

/* MULTI-THREADED SOLUTION that allows multiple alarms to
 *  be set and running simultaneously
 */

/* Users can enter a timer amount (in seconds) and
 *  a string to display once the timer expires
 *
 * ==> 3 wake up
 * ALARM: wake up
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define MAXLINE 80

typedef struct
{
  int seconds;
  char msg[MAXLINE];
}
alarm_t;

void * alarm_thread_function( void * arg )
{
  alarm_t * alarm = (alarm_t *)arg;

  pthread_t tid = pthread_self();   /* get my thread ID */

  /* pthread_detach() enables pthreads to reclaim the
   *  thread resources when this thread terminates,
   *   i.e., do *not* leave a "zombie" thread behind
   *
   * we therefore do not need to call pthread_join()
   */
  pthread_detach( tid );

  printf( "CHILD %lu: Alarm set for %d seconds\n", tid, alarm->seconds );
  sleep( alarm->seconds );
  fprintf( stderr, "CHILD: ALARM (%d): %s\n", alarm->seconds, alarm->msg );

  /* deallocate the memory that the main thread allocated for this thread */
  free( alarm );

  return NULL;  /* the thread will terminate its execution here */
}

int main()
{
  char * line = calloc( MAXLINE + 1, sizeof( char ) );
  char * msg = calloc( MAXLINE + 1, sizeof( char ) );

  while ( 1 )
  {
    int seconds;

    printf( "Set alarm (<sec> <msg>) -- 0 to exit: " );

    if ( fgets( line, MAXLINE + 1, stdin ) == NULL ) break;

    if ( strlen( line ) <= 1 ) continue;  /* skip blank lines */

    if ( sscanf( line, "%d %[^\n]", &seconds, msg ) < 2 || seconds < 0 )
    {
      if ( seconds == 0 ) break;
      fprintf( stderr, "ERROR: invalid alarm request\n" );
    }
    else
    {
      pthread_t tid;   /* thread ID */

#if 0
      /* create the next alarm */
      alarm_t arg;                  /* static alloc: +--------+ */
      arg.seconds = seconds;        /*               |        | */
      strcpy( arg.msg, msg );       /*               +--------+ */
#else
      /* dynamically allocate memory for the next alarm thread */
      alarm_t * arg = malloc( sizeof( alarm_t ) );
      arg->seconds = seconds;
      strcpy( arg->msg, msg );
#endif

      /* create a child thread to actually set/run the alarm */
      int rc = pthread_create( &tid, NULL, alarm_thread_function, arg );

      if ( rc != 0 )
      {
        fprintf( stderr, "ERROR: pthread_create() failed\n" );
        return EXIT_FAILURE;
      }

      /* MAIN (PARENT) THREAD */
      *msg = '\0';

      /* TO DO: somehow call waitpid() here with WNOHANG... */
      usleep( 1000 );
    }
  }

  free( msg );
  free( line );

  return EXIT_SUCCESS;
}
