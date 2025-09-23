/* pipe.c */

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

  /* write data to the pipe */
  int bytes_written = write( pipefd[1], "ABCDEFGHIJKLMNOPQRSTUVWXYZ", 26 );
  printf( "Wrote %d bytes to the pipe\n", bytes_written );

  /* read data from the pipe */
  char tmp[32];
  int bytes_read = read( pipefd[0], tmp, 10 );
  tmp[bytes_read] = '\0';  /* assume that we are reading char data */
  printf( "Read %d bytes: \"%s\"\n", bytes_read, tmp );

  bytes_read = read( pipefd[0], tmp, 10 );
  tmp[bytes_read] = '\0';
  printf( "Read %d bytes: \"%s\"\n", bytes_read, tmp );

  bytes_read = read( pipefd[0], tmp, 10 );
  tmp[bytes_read] = '\0';
  printf( "Read %d bytes: \"%s\"\n", bytes_read, tmp );

#if 0
  close( pipefd[1] );  /* close the "write" end of the pipe */
  /* this could/should move up to be directly after the write() call... */
#endif

  /* What happens with this fourth read() call?
   * ANSWER: at this point, the pipe buffer is empty...
   *  ...so this next read() call will BLOCK indefinitely
   *   (in this case forever) because there is no data on the
   *    pipe to read --- AND there is at least one open/active
   *     "write" descriptor on the pipe.
   *
   * FIX: uncomment the close() call above...
   */
  bytes_read = read( pipefd[0], tmp, 10 );   /* this read() call BLOCKS */
  tmp[bytes_read] = '\0';
  printf( "Read %d bytes: \"%s\"\n", bytes_read, tmp );

  bytes_read = read( pipefd[0], tmp, 10 );
  tmp[bytes_read] = '\0';
  printf( "Read %d bytes: \"%s\"\n", bytes_read, tmp );

  bytes_read = read( pipefd[0], tmp, 10 );
  tmp[bytes_read] = '\0';
  printf( "Read %d bytes: \"%s\"\n", bytes_read, tmp );

  return EXIT_SUCCESS;
}