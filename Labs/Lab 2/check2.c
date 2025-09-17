#include <stdio.h>
#include <stdlib.h>
int main()
{
    /* The 0x prefix indicates a hexadecimal number... */
    int z = 0x76654B;
    char * c = (char *)&z;
    printf( "%c", *c++ );
    printf( "%c", *c++ );
    printf( "%c", *c++ );
    printf( "%c\n", *c++ );
    return EXIT_SUCCESS;
}