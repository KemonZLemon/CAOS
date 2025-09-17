<<<<<<< HEAD
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
=======
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
>>>>>>> 053a24431f20f9100c5412c92269c0c71dba221a
}