/* dynamic-allocation-v2-pointer.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main()
{
    /* dynamically allocate 100 bytes on the runtime heap */
    char * s = malloc(100);
    /* uninitialized chunk of memory */

    /* dynamically allocate 100 bytes on the runtime heap */
    char * t = calloc(100, sizeof(char));
    /* chunk of memory initialized to all '\0' bytes */

    printf("s: \"%s\" -- t: \"%s\"\n", s, t);

    *(s + 0) = 'A';
    *(s + 1) = 'B';
    *(s + 2) = 'C';
    *(s + 3) = '\0';

    *(s + 20) = 'X';
    *(s + 21) = 'Y';
    *(s + 22) = 'Z';
    *(s + 23) = '\0';

    printf("s: \"%s\" -- t: \"%s\"\n", s, t);
    printf("&s[20]: \"%s\" -- t: \"%s\"\n", s + 20, t);
    printf("s + 20: \"%s\" -- t: \"%s\"\n", s + 20, t);

    free(t);
    free(s);

    /* dynamically allocate an array of 32 integers on the runtime heap */
    int * numbers = calloc(32, sizeof(int));  /* 128 bytes allocated */
    /* chunk of memory initialized to all zero bytes */

    *(numbers + 16) = 1234;
    printf("numbers[16]: %d\n", *(numbers + 16));
    printf("*(numbers + 16): %d\n", *(numbers + 16));
    /* The numbers + 16 calculation adds 64 to the base memory addr */
    /*     numbers + (16 * sizeof(int)) == numbers + 64            */

    *(numbers + 16) = 2345;
    printf("numbers[16]: %d\n", *(numbers + 16));
    printf("*(numbers + 16): %d\n", *(numbers + 16));

    free(numbers);

    return EXIT_SUCCESS;
}
