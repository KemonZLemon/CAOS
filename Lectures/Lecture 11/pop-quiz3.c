/* pop-quiz3.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

  int main()
  {
    printf( " -=-=-\n" );

    /* if we run this as ./a.out (on the terminal),
     *  this immediately prints " -=-=-\n" because the
     *   '\n' char flushes the stdout (fd 1) buffer
     *
     * if we instead run this as ./a.out>output.txt,
     *  then " -=-=-\n" is in the stdout buffer only
     *   (in memory) ---- therefore, we call fork()
     *    and this buffer is copied/inherited by the
     *     child process
     */


    pid_t p = fork();
    if ( p == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

    if ( p == 0 )
    {
      printf( "( o o )\n" );
      return 'Y';
    }
    else
    {
      int status;
      waitpid( p, &status, 0 );
      printf( " - %c -\n", WEXITSTATUS( status ) );
    }

    printf( ". |=| .\n%04d\n", p );
    return EXIT_SUCCESS;
  }






#if 0
 -=-=-
( o o )
 - Y -
. |=| .
<pid>
#endif
