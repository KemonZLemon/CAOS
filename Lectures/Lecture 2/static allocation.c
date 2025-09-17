#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int x = 5;  /* x is statically allocated (on the stack)*/
                /* (4 bytes are allocated on the stack)*/

    printf("x is %d\n", x);
    printf("sizeof(x) is %lu bytes\n", sizeof(x));
    printf("sizeof(int) is %lu bytes\n", sizeof(int));

    int * q = NULL; /* q is statically allocated(on the stack) */
#if 0
    printf("*q is %d\n", *q);
#endif

    printf("sizeof(q) is %lu bytes\n", sizeof(q));
    printf("sizeof(int*) is %lu bytes\n", sizeof(int*));
    printf("sizeof(*q) is %lu bytes\n", sizeof(*q));

    char * c = NULL; //c is statically allocated on the stack
    printf("sizeof(c) is %lu bytes\n", sizeof(c));
    printf("sizeof(char*) is %lu bytes\n", sizeof(char*));
    printf("sizeof(*c) is %lu bytes\n", sizeof(*c));
    printf("sizeof(char) is %lu bytes\n", sizeof(char));

    return EXIT_SUCCESS;
}