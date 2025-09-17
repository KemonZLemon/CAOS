/* scanf.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main()
{
    float x;
    printf( "Enter value: " );
    scanf( "%f", &x );
    printf( "Thanks, x is %.2f\n", x );
    return EXIT_SUCCESS;
}