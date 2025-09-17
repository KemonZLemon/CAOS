#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    /*not that malloc() returns a void*, which is essentially a placeholder pointer,
     *i.e. a memory address that cannot be dereferenced. */

    //dynamically allocate 16 bytes on the runtime heap
    char* path = malloc(16);    //on the stack path is a memory address
    //char* path = malloc(160000000000000); gives error
    if(path == NULL) {
        perror("malloc() failed");
        return EXIT_FAILURE;
    }
    printf("sizeof(path) is %lu\n", sizeof(path)); //will print 8 bytes bc path is a pointer,
                                                         //even if pointing to 18 bytes on the heap
    //copy a string
    strcpy(path, "/cs/goldsd/f25"); //set the 16 bytes to this string along with \0
    printf("path is \"%s\" (strlen is %lu)\n", path, strlen(path));
    free(path); //deallocate the dynamically allocated memory

    double* value = malloc(sizeof(double));
    if(value==NULL) {
        perror("malloc() failed");
        return EXIT_FAILURE;
    }

    *value = 123.45;
    printf("value points to %lf\n", *value);
    printf("sizeof(value) is  %lu\n", value);
    free(value);




    return EXIT_SUCCESS;
}