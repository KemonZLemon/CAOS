/* fork-with-exec.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

/* use fork() and execl() to execute:
 *
 * bash$ ps -f  --or--  ps -ef  --or--  ls -a  --or--  etc.
 *
 */

int main()
{
  pid_t p1;

  p1 = fork();
  if ( p1 == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

  if ( p1 == 0 )  /* CHILD PROCESS */
  {
#if 0
    printf( "CHILD: executing ls -al nosuchfile.txt...\n" );

                          /* argv[0], argv[1],           argv[2], argv[3] */
    execl( "/usr/bin/ls",       "ls",    "-al", "nosuchfile.txt", NULL );
                          /* argv[0], argv[1], argv[2] */
    execl( "/usr/bin/ls",       "ls",    "-al",    NULL );
#endif
    printf( "CHILD: executing ps -f...\n" );

                          /* argv[0], argv[1], argv[2] */
    execl( "/usr/bin/ps",       "ps",    "-f"    NULL );
#if 0
                          /* argv[0], argv[1], argv[2] */
    execl( "/usr/bin/psxyz",       "ps",    "-ef",    NULL );
#endif

    /* we should not get here in this code! */
    /* -- if we do end up here, then execl() returned -1 due to failure */
    perror( "CHILD: execl() failed" );
    return EXIT_FAILURE;
    /***************************************/
  }
  else /* p1 > 0    PARENT PROCESS */
  {
    /* wait (BLOCK indefinitely) for my child process to end... */
    int status;
    pid_t child_pid = waitpid( p1, &status, 0 );
  
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
  }

  return EXIT_SUCCESS;
}
