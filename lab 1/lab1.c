/* LAB 1 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <stdbool.h>

void triangle(int size) {
    /* ERROR CHECKING */
    if (size % 2 == 0) {
        printf("ERROR - TRY AGAIN!\n");
        return;
    }
    /* ACTUAL FUNCTION */
    for (int i = 1; i <=size; i = i+2) {
        int space = (size-i)/2;
        for(int k = 0; k < space; ++k){
            printf(" ");
        }
        for (int j = 0; j < i; j++) {
            printf("^");
        }
        printf("\n");
    }
}

int main() {
    /* CHECKPOINT 1
    printf("%3c %-15s %s", 'x', "ln(x)", "e^x\n");
    printf("--- --------------- ------------------\n");
    int x = 0;

    for ( x = 0 ; x < 8 ; x++ ) {
        printf( "%3.0f %.13f %.13f\n", pow(2,x), log( pow(2,x) ), exp( pow(2,x) ) );
    }
    printf("--- --------------- ------------------\n");
    */

    /* CHECKPOINT 2*/
    int size;
    while(true) {
        printf("Enter positive integer (-1 to exit): ");

        scanf("%d", &size);
        if(size == -1) break;
        triangle(size);
    }

    return 0;
}