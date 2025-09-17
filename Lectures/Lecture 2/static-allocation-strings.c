
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

int main() {
    char name[6] = "DAvid"; // this array has the string "DAvid\0"
                            // \0=null terminator (required)
    printf("Hello, %s\n", name);
    name[1] = 'a';
    printf("Hello again, %s\n", name);

    printf("Length of name is  %ld\n", strlen(name));

#if 0
    char xyz[5] = "QRSTU";
    printf("xyz is  %s\n", xyz);
#endif
    return EXIT_SUCCESS;
}