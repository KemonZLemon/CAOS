<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    //dynamically allocate 100 bytes on the runtime heap
    char * s = malloc(100);
    //uninitialized chunk of memory


    //dynamically allocate 100 bytes on the runtime heap
    char * t = calloc(100, sizeof(char));
    //chunk of memory initialozed to all "\0" bytes

    printf("s: \"%s\" -- t: \"%s\"\n", s, t);

    s[0] = 'A';
    s[1] = 'B';
    s[2] = 'C';
    s[3] = '\0';

    s[20] = 'X';
    s[21] = 'Y';
    s[22] = 'Z';
    s[23] = '\0';

    printf("s: \"%s\" -- t: \"%s\"\n", s, t);
    printf("&s[20]: \"%s\" -- t: \"%s\"\n", &s[20], t);
    printf("s + 20: \"%s\" -- t: \"%s\"\n", s + 20, t); //pointer arithmetic

    //dynamically allocate an array of 32 ints on the runtime heap
    int * numbers = calloc(32, sizeof(int)); //128 bytes allocated
    //chunk of memory intitialized to all zero bytes
    numbers[16] = 1234;
    printf("numbers[16]: %d\n", numbers[16]);
    printf("numbers + 16: %d\n", *(numbers + 16)); //+16 adds 64 bytes to memory address
    *(numbers + 16) = 2345;
    printf("numbers[16]: %d\n", numbers[16]);
    printf("numbers + 16: %d\n", *(numbers + 16));

    free(t);
    free(s);
    free(numbers);
=======
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
    //dynamically allocate 100 bytes on the runtime heap
    char * s = malloc(100);
    //uninitialized chunk of memory


    //dynamically allocate 100 bytes on the runtime heap
    char * t = calloc(100, sizeof(char));
    //chunk of memory initialozed to all "\0" bytes

    printf("s: \"%s\" -- t: \"%s\"\n", s, t);

    s[0] = 'A';
    s[1] = 'B';
    s[2] = 'C';
    s[3] = '\0';

    s[20] = 'X';
    s[21] = 'Y';
    s[22] = 'Z';
    s[23] = '\0';

    printf("s: \"%s\" -- t: \"%s\"\n", s, t);
    printf("&s[20]: \"%s\" -- t: \"%s\"\n", &s[20], t);
    printf("s + 20: \"%s\" -- t: \"%s\"\n", s + 20, t); //pointer arithmetic

    //dynamically allocate an array of 32 ints on the runtime heap
    int * numbers = calloc(32, sizeof(int)); //128 bytes allocated
    //chunk of memory intitialized to all zero bytes
    numbers[16] = 1234;
    printf("numbers[16]: %d\n", numbers[16]);
    printf("numbers + 16: %d\n", *(numbers + 16)); //+16 adds 64 bytes to memory address
    *(numbers + 16) = 2345;
    printf("numbers[16]: %d\n", numbers[16]);
    printf("numbers + 16: %d\n", *(numbers + 16));

    free(t);
    free(s);
    free(numbers);
>>>>>>> 053a24431f20f9100c5412c92269c0c71dba221a
}