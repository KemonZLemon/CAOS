
/* fork.c */

/* fork() is called to create a new (child) process */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    int x = 13;
    char * y = malloc( 13 );
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
    }
    else /* p > 0    PARENT process runs this code */
    {
        printf( "PARENT: My new child process id (PID) is %d\n", p );
        printf( "PARENT: My process id (PID) is %d\n", getpid() );
    }

    return EXIT_SUCCESS;
}

#if 0
HOW MANY POSSIBLE OUTPUTS ARE THERE FOR THIS CODE...?

goldsd3@linux-new:~/f25/csci2800$ ./a.out
CHILD: Happy birthday to me!
PARENT: My new child process id (PID) is 1849971
PARENT: My process id (PID) is 1849970
CHILD: My process id (PID) is 1849971
goldsd3@linux-new:~/f25/csci2800$ ./a.out
PARENT: My new child process id (PID) is 1850022
PARENT: My process id (PID) is 1850021
CHILD: Happy birthday to me!
CHILD: My process id (PID) is 1850022
#endif