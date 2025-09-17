/* fd-read.c */

/**
 * A "file" is simply a stream of bytes --- read and/or write
 *
 * file descriptor (fd)
 *
 * -- a small non-negative integer used in a variety of system calls
 *     to refer to an open file (i.e., file stream or byte stream)
 *
 * -- each process has a file descriptor table associated with it
 *     that keeps track of its open (file) descriptors
 *
 * fd         C++   Java        C
 *  0 stdin   cin   System.in   scanf(), fgetc(), read(), ...
 *  1 stdout  cout  System.out  printf(), write(), ...
 *  2 stderr  cerr  System.err  write(), perror( "calloc() failed" );
 *                                       fprintf( stderr, "ERROR: ...\n" );
 *
 * stdout and stderr (by default) are both displayed on the terminal
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

int main()
{
    char * name = "testfile.txt";

    int fd = open( name, O_RDONLY );
    if ( fd == -1 ) { perror( "open() failed" ); return EXIT_FAILURE; }

    printf( "fd is %d\n", fd );

    /* fd table for this running process:
     *
     *  0 stdin
     *  1 stdout
     *  2 stderr
     *  3 testfile.txt (O_RDONLY)
     */

    /* read from the file */
    char buffer[32];
    int rc = read( fd, buffer, 16 );  /* TO DO: should also check if rc == -1 */
    buffer[rc] = '\0';   /* assume the data are characters... */
    printf( "read() returned %d -- buffer \"%s\"\n", rc, buffer );

    rc = read( fd, buffer, 16 );
    buffer[rc] = '\0';
    printf( "read() returned %d -- buffer \"%s\"\n", rc, buffer );

    rc = read( fd, buffer, 16 );
    buffer[rc] = '\0';
    printf( "read() returned %d -- buffer \"%s\"\n", rc, buffer );

    while ( 1 )
    {
        rc = read( fd, buffer, 16 );
        buffer[rc] = '\0';
        printf( "read() returned %d -- buffer \"%s\"\n", rc, buffer );
        sleep( 3 );   /* delay (suspend the process) for 3 seconds... */
    }

    close( fd );

    return EXIT_SUCCESS;
}