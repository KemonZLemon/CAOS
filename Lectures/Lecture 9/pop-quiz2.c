/* pop-quiz2.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

  int main( int argc, char ** argv )
  {
    if ( argc != 2 ) { return EXIT_FAILURE; }
    int q2 = atoi( *(argv + 1) );

    printf( "ABCD\n" );

    pid_t p = fork();
    if ( p == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

    printf( "EFGH%04d\n", q2 );

    if ( p == 0 )
    {
      printf( "IJKL%04d\n", getpid() );
    }
    else
    {
      printf( "MNOP%04d\n", getpid() );
    }

    printf( "QRST\n" );

    return EXIT_SUCCESS;
  }
