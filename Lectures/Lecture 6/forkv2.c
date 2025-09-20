/* fork-v2.c */

/* fork() is called to create a new (child) process */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int x = 13;
    char * y = malloc( x );
    strcpy( y, "HELLOWORLD" );

#if 0
    printf( "%s\n", y );
#else
    printf( "%s", y );   /* without the '\n' char, stdout buffer: "HELLOWORLD"
                          *  and the stdout buffer is copied in the child
                          *   process (refer to buffering.c)
                          */
#endif

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
    }
    else /* p > 0    PARENT process runs this code */
    {
        printf( "PARENT: My new child process id (PID) is %d\n", p );
        printf( "PARENT: My process id (PID) is %d\n", getpid() );
    }

    printf( "PID %d: All done\n", getpid() );

    return EXIT_SUCCESS;
}