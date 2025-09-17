
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    char** names;
#if 0
    names = malloc(64 * sizeof(char*)); //dynamically allocate 512 bytes
#else
    names = calloc(64, sizeof(char*));
#endif
    names[2] = calloc(7, sizeof(char)); //dynamically allocate 7 bytes

    strcpy(names[2], "SPARKS");
    strcpy(*(names + 2), "SPARKS");

    sprintf(*(names + 2), "SPARKS");

    printf("Let's go %s!\n", names[2]);
    printf("Let's go, %s!\n", *(names+ 2));

    names[2] = realloc(names[2], 4); //reduce to 4 bytes
    printf("Let's go, %s!\n", names[2]);

    names[2] = realloc(names[2], 100); //expand by 100 bytes
    sprintf(*(names+2), "SPARKS will make the playoffs");
    printf("Let's go! %s!\n", names[2]);

    printf("%c\n", names[2][3]);
    printf("%c\n", *(*(names +2) +3));
    printf("%s\n", *(names +2) +3);

    free(*(names +2)); //same as free(names[2]);
    free(names);
    return EXIT_SUCCESS;

}