/* signals.c */

/* Signals give us an asynchronous means of inter-process communication (IPC) */

/* TO DO: check out "man 2 signal" and "man 7 signal" */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>

void signal_handler( int sig )
{
  printf( "Rcvd signal %d\n", sig );

  if ( sig == SIGINT )
  {
    printf( "Stop hitting CTRL-C and answer the question!\n" );
  }
  else if ( sig == SIGTERM )
  {
    printf( "Sorry, I'm not shutting down!\n" );
  }
  else if ( sig == SIGUSR1 )
  {
    printf( "Okay, got it, reloading the config file...(not really)\n" );
  }
}

int main()
{
  signal( SIGINT, SIG_IGN );  /* ignore SIGINT (CTRL-C) */
  signal( SIGTERM, SIG_IGN ); /* ignore SIGTERM */

  signal( SIGINT, signal_handler );
  signal( SIGTERM, signal_handler );

  char name[128];
  printf( "Enter your name: " );
  scanf( "%s", name );   /* BUFFER OVERFLOW POSSIBLE HERE... */
  printf( "Hi, %s\n", name );

  signal( SIGINT, SIG_DFL );  /* restore default behavior for SIGINT */

  printf( "Enter your name again: " );
  scanf( "%s", name );   /* BUFFER OVERFLOW POSSIBLE HERE... */
  printf( "Hi, %s\n", name );


  return EXIT_SUCCESS;
}