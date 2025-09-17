<<<<<<< HEAD
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int x;
    printf("Enter value: ");
    scanf("%d", &x);
    printf("Thanks, x is %d\n");

    //bad idea: reading chars with scanf because we don't know what user will do
    //in this case if more than 15 chars then will buffer overflow
    char name[16];
    printf("Enter name: ");
    scanf("%s", name);
    printf("Hi %s\n", name);
=======
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int x;
    printf("Enter value: ");
    scanf("%d", &x);
    printf("Thanks, x is %d\n");

    //bad idea: reading chars with scanf because we don't know what user will do
    //in this case if more than 15 chars then will buffer overflow
    char name[16];
    printf("Enter name: ");
    scanf("%s", name);
    printf("Hi %s\n", name);
>>>>>>> 053a24431f20f9100c5412c92269c0c71dba221a
}