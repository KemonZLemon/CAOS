#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/* When printing to the terminal (shell) via stdout (fd 1),
 *  output is buffered in memory and the newline '\n' character
 *   will empty ("flush") the stdout buffer,
 *    i.e., output everything that has been stored in
 *           the stdout buffer so far...
 *
 * ^^^^^ this is called line-based buffering (default for fd 1)
 *
 * When we instead output (redirect) fd 1 to a file...
 *
 *   bash$ ./a.out > output.txt
 *
 * ...the '\n' character no longer flushes the buffer
 *
 * ^^^^^ this is called block-based buffering (or fully buffered)
 *
 * A third type of buffering is non-buffered (unbuffered),
 *  which is what is used for stderr (fd 2)
 *
 *   e.g., Segmentation fault (core dumped)
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    printf( "how about here?!" ); /* stdout buffer: "how about here?!" */
    /* (this buffer is in memory...)     */

    int * x = NULL;

    printf( "HERE0\nXYZ" );   /* stdout buffer: "how about here?!HERE0\n" */
    /* (the '\n' char causes the buffer to be flushed */
    /* stdout buffer: "XYZ" */

    *x = 1234;

    printf( "HERE1\n" );

    printf( "*x is %d\n", *x );

    printf( "HERE2\n" );

    return EXIT_SUCCESS;
}