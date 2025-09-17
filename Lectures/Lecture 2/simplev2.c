<<<<<<< HEAD
/* simple.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //add -lm when compiling
#include <math.h>

#define LOOP_COUNT 8
/* bash$ gcc -Wall -Werror simplev2.c -lm*/
int main()
{
    //another way to commend code out is by putting them between #if 0 and #endif
    #if 0
    float y;
    printf("variable y is %f\n", y);
    #else
     float y = 123.45;
    printf("variable y is %f\n", y);
    #endif
    int x;


    for ( x = 1 ; x < LOOP_COUNT ; x++ )
    {
        printf( "%3d %f %20.15f\n", x, sqrt( x ), sqrt( x ) );
        printf( "%3d %f %-20.15f\n", x, sqrt( x ), sqrt( x ) );
        printf( "%d %f %20.15f\n", x, sqrt( x ), sqrt( x ) );

    }

    return EXIT_SUCCESS;   /* bash$ echo $? */
=======
/* simple.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>   //add -lm when compiling
#include <math.h>

#define LOOP_COUNT 8
/* bash$ gcc -Wall -Werror simplev2.c -lm*/
int main()
{
    //another way to commend code out is by putting them between #if 0 and #endif
    #if 0
    float y;
    printf("variable y is %f\n", y);
    #else
     float y = 123.45;
    printf("variable y is %f\n", y);
    #endif
    int x;


    for ( x = 1 ; x < LOOP_COUNT ; x++ )
    {
        printf( "%3d %f %20.15f\n", x, sqrt( x ), sqrt( x ) );
        printf( "%3d %f %-20.15f\n", x, sqrt( x ), sqrt( x ) );
        printf( "%d %f %20.15f\n", x, sqrt( x ), sqrt( x ) );

    }

    return EXIT_SUCCESS;   /* bash$ echo $? */
>>>>>>> 053a24431f20f9100c5412c92269c0c71dba221a
}