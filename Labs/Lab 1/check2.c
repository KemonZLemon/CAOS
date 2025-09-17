#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void trianglev2(int size) {
    char *row = (char *)malloc((size) * sizeof(char));
    if (row == NULL) {
        printf("Memory allocation failed!\n");
        return;
    }
    memset(row, ' ', size);
    int left = size / 2;
    int right = size / 2;
    while (left >= 0 && right < size) {
        row[left] = '^';
        row[right] = '^';
        printf("%s\n", row);
        left--;
        right++;
    }
    free(row);
}

void triangle(int size) {
    int i, j;
    int rows = (size + 1) / 2;
    for(i = 1; i <= rows; i++) {
        for(j = 1; j <= rows - i; j++) {
            printf(" ");
        }

        for(j = 1; j <= 2 * i - 1; j++) {
            printf("^");
        }
        printf("\n");
    }
}

int main() {
    int n;
    while(1) {
        printf("Enter positive integer(-1 to exit):");
        scanf("%d", &n);
        if(n == -1) {
            break;
        } else if(n>0 && n%2 == 1){
            trianglev2(n);
        } else {
            printf("ERROR - TRY AGAIN!\n");
        }
    }
    return 0;
}

