<<<<<<< HEAD
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char * word = (char*)calloc(17, sizeof(char));
    if (word == NULL) {
        printf("Memory allocation error");
        return 1;
    }

    int count = 0;
    int letter;

    while(count < 16 && (letter = fgetc(stdin)) != EOF) {
        if(isalnum(letter)) {
            word[count] = letter;
            count++;
        }
    }

    if(count >= 16) {
        word = (char*)realloc(word, 49 * sizeof(char));
        while(count < 48 && (letter = fgetc(stdin)) != EOF){
            if(isalnum(letter)) {
                word[count] = letter;
                count++;
            }
        }
    }

    printf("Word: %s\n", word);
    free(word);
    return EXIT_SUCCESS;
}
=======
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>

int main() {
    char * word = (char*)calloc(17, sizeof(char));
    if (word == NULL) {
        printf("Memory allocation error");
        return 1;
    }

    int count = 0;
    int letter;

    while(count < 16 && (letter = fgetc(stdin)) != EOF) {
        if(isalnum(letter)) {
            word[count] = letter;
            count++;
        }
    }

    if(count >= 16) {
        word = (char*)realloc(word, 49 * sizeof(char));
        while(count < 48 && (letter = fgetc(stdin)) != EOF){
            if(isalnum(letter)) {
                word[count] = letter;
                count++;
            }
        }
    }

    printf("Word: %s\n", word);
    free(word);
    return EXIT_SUCCESS;
}
>>>>>>> 053a24431f20f9100c5412c92269c0c71dba221a
