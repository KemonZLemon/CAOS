/* lab05-checkpoint1.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    int * pipefd = malloc( 2 * sizeof ( int ) );
    if ( pipefd == NULL ) { perror( "malloc() failed" ); return EXIT_FAILURE; }

    int rc = pipe( pipefd );
    if ( rc == -1 ) { perror( "pipe() failed" ); return EXIT_FAILURE; }

    pid_t p = fork();
    if ( p == -1 ) { perror( "fork() failed" ); return EXIT_FAILURE; }

    if ( p == 0 )  /* CHILD PROCESS */
    {
        close( *(pipefd + 1) );  /* close the write end of the pipe */
        int bytes_read = 0;
        do
        {
            unsigned int value = 0;
            bytes_read = read( *pipefd, &value, sizeof( unsigned int ) );
            if ( bytes_read == -1 ) { perror( "CHILD: read() failed" ); return EXIT_FAILURE; }
            if ( bytes_read > 0 )
                printf( "CHILD: Read %u from pipe (%d bytes)\n", value, bytes_read );
        }
        while ( bytes_read != 0 );
        close( *pipefd );        /* close the read end of the pipe */
    }
    else /* p > 0   PARENT PROCESS */
    {
        close( *pipefd );        /* close the read end of the pipe */
        unsigned int value = 0;
        do
        {
            printf( "PARENT: Enter positive integer (0 to exit): " );
            if ( scanf( "%d", &value ) == 1 )
            {
                if ( value > 0 )
                {
                    int bytes_written = write( *(pipefd + 1), &value, sizeof( unsigned int ) );
                    if ( bytes_written == -1 ) { perror( "PARENT: write() failed" ); return EXIT_FAILURE; }
                    printf( "PARENT: Wrote %u to pipe (%d bytes)\n", value, bytes_written );
                }
            }
        }
        while ( value != 0 );
        close( *(pipefd + 1) );  /* close the write end of the pipe */
    }
    //bug: lost 8 bytes
    free( pipefd );
    return EXIT_SUCCESS;
}