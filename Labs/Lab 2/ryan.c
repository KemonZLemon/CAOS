
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    int count = 0;
    char* characters = calloc(16, sizeof( char ) );
    char* tmp = characters;

    printf("Enter a string of characters: ");


    int c = fgetc(stdin);


    while(c != EOF && count != 16){
        if(isalnum(c)){
            *tmp = c;
            tmp++;
            count++;
        }
        c = fgetc(stdin);
    }


    printf("String: %s\n", characters);
    free(characters);
    tmp = NULL;
}