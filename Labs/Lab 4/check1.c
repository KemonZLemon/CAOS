/*
In the code below, how many bytes are statically allocated? How many bytes
are dynamically allocated in each call to malloc() and calloc()? And how many bytes are
uninitialized before the program ends?
What is the exact output of the given code?
For each expression in which we use square bracket notation, rewrite using pointer arithmetic.
How many bytes are added to the original pointer in each case?
Finally, add the appropriate calls to free() to ensure no memory leaks.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main()
{
    int q = -3;
    char * name = malloc( 16 );
    int * x = calloc( -q, sizeof( int ) );
    int * numbers = calloc( 16, sizeof( int ) );
    double * values = calloc( 16, sizeof( double ) );
    strcpy( name, "QRSTUVWXYZ" );
    *(x + 0) = 22;  //x[0] = 22;
    *(x + 1) = 24;  //x[1] = 24;
    *(numbers + 5) = *(x + 0) - *(x + 1);   //numbers[5] = x[0] - x[1];
    *(values + 5) = *(x + 0) / *(x + 1);    //values[5] = x[0] / x[1];
    sprintf( name, "ABCD%04dEFGH", x[0] );
    printf( "%12s\n", &name[5] );
    printf( "%*d%%", q, numbers[5] );
    printf( "%8.4lf\n", values[5] );

    free( name );
    free( numbers );
    free( values );
    free( x );
    return EXIT_SUCCESS;
}
/*
 *how many bytes are statically allocated?  4 bytes
 *How many bytes are dynamically allocated in each call to malloc() and calloc()?   16 + 12 + 64 + 128 = 220 bytes
 *how many bytes are uninitialized before the program ends?     16 bytes
 *What is the exact output of the given code?
 *022EFGH
 *-2 %  0.0000
 *
 */

