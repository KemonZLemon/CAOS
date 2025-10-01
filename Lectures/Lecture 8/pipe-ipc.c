/* pipe-ipc.c */

/* A "pipe" is a unidirectional communication channel -- man 2 pipe */
/*                                                    -- man 7 pipe */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
  int pipefd[2];  /* array to hold the two pipe (file) descriptors:
                   *  pipefd[0] is the "read" end of the pipe
                   *  pipefd[1] is the "write" end of the pipe
                   */

  /* create the pipe */
  int rc = pipe( pipefd );
  if ( rc == -1 ) { perror( "pipe() failed" ); return EXIT_FAILURE; }

  /* fd table:
   *
   *  0: stdin
   *  1: stdout
   *  2: stderr                +--------+ think of this buffer as a
   *  3: pipefd[0] <===READ====| buffer |  temporary hidden chunk
   *  4: pipefd[1] ====WRITE==>| buffer |   or memory...
   *                           +--------+ we will be able to share this
   *                                       pipe buffer with other processes...
   */

  printf( "Created pipe: pipefd[0] is %d and pipefd[1] is %d\n",
          pipefd[0], pipefd[1] );

  /* fork() duplicates the fd table to the child process */
  pid_t p = fork();
  if ( p == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

  /* fd table:
   *
   *  <PARENT>                                        <CHILD>
   *  0: stdin                                        0: stdin
   *  1: stdout                                       1: stdout
   *  2: stderr                +--------+             2: stderr
   *  3: pipefd[0] <===READ====| buffer |====READ===> 3: pipefd[0]
   *  4: pipefd[1] ====WRITE==>| buffer |<==WRITE==== 4: pipefd[1]
   *                           +--------+
   */

  /* A pipe is supposed to be undirectional...
   * ...which means we should close unused ends of the pipe
   */

  if ( p == 0 )  /* CHILD PROCESS */
  {
    /* write data to the pipe */
    int bytes_written = write( pipefd[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
    printf( "CHILD: Wrote %d bytes to the pipe\n", bytes_written );
  }
  else /* p > 0   PARENT PROCESS */
  {
    /* read data from the pipe */
    char tmp[32];
    int bytes_read = read( pipefd[0], tmp, 10 );
    tmp[bytes_read] = '\0';  /* assume that we are reading char data */
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, tmp );

    bytes_read = read( pipefd[0], tmp, 10 );
    tmp[bytes_read] = '\0';
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, tmp );

    bytes_read = read( pipefd[0], tmp, 10 );
    tmp[bytes_read] = '\0';
    printf( "PARENT: Read %d bytes: \"%s\"\n", bytes_read, tmp );

    /* if we called read() a fourth time here, read() would block... */
  }

  /* WHAT IS THE EXACT OUTPUT OF THIS CODE?  SHOW ALL POSSIBILITIES! */

  return EXIT_SUCCESS;
}

#if 0
EXACT OUTPUT:
                Created pipe: pipefd[0] is 3 and pipefd[1] is 4
                                    fork()
                                   /      \
                        <parent>  /        \  <child>
                                 /          \
===================================         ===================================
PARENT: Read 10 bytes: "ABCDEFGHIJ"         CHILD: Wrote 26 bytes to the pipe
PARENT: Read 10 bytes: "KLMNOPQRST"
PARENT: Read 6 bytes: "UVWXYZ"
===================================         ===================================

#endif

