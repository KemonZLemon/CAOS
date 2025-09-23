/* fork-v3.c */

/* fork() is called to create a new (child) process */

/* reminder to review 09-23-notes.txt in conjunction with this code... */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main()
{
  int x = 13;
  char * y = malloc( x );
  strcpy( y, "HELLOWORLD" );

  pid_t p; /* pid_t is essentially int */

  p = fork();  /* create a new (child) process */
  if ( p == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

  /* we now have TWO separate processes running
   *  independently of one another...
   */

  if ( p == 0 )  /* CHILD process runs this code */
  {
    printf( "CHILD: Happy birthday to me!\n" );
    printf( "CHILD: My process id (PID) is %d\n", getpid() );
#if 1
    sleep( 20 );   /* this will show in the ps list that this child process */
    /*  is adopted by top-level process ID 1 ... */
#endif
  }
  else /* p > 0    PARENT process runs this code */
  {
#if 1
    sleep( 20 );
#endif
    printf( "PARENT: My new child process id (PID) is %d\n", p );
    printf( "PARENT: My process id (PID) is %d\n", getpid() );

#if 1
    /* wait (BLOCK indefinitely) for my child process to end... */
    int status;
    pid_t child_pid = waitpid( p, &status, 0 );

    printf( "PARENT: child process %d terminated...\n", child_pid );

    if ( WIFSIGNALED( status ) )
    {
      printf( "PARENT: ...abnormally (killed by a signal)\n" );
    }
    else if ( WIFEXITED( status ) )
    {
      int exit_status = WEXITSTATUS( status );
      printf( "PARENT: ...normally with exit status %d\n", exit_status );
    }
#endif
  }

  printf( "PID %d: All done\n", getpid() );

  return EXIT_SUCCESS;
}