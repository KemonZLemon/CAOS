
/* simple.c */

#include <stdio.h>
#include <stdlib.h>
//#include <unistd.h>   //add -lm when compiling
#include <math.h>

int main()
{
    int x;
    /*   float y; */

    for ( x = 1 ; x < 8 ; x++ )
    {
        printf( "%3d %f %20.15f\n", x, sqrt( x ), sqrt( x ) );
    }

    return EXIT_SUCCESS;   /* bash$ echo $? */

}