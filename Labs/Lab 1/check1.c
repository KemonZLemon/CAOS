#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>

int main() {
printf("%3c %s %14s", 'x', "ln(x)", "e^x\n");
printf("--- --------------- ------------------\n");
int x = 0;

for(x = 0; x < 8; x++){
printf("%f %.13f %.13f\n", pow(2, x), log(pow(2,x)), exp(pow(2,x)));
}
printf("--- --------------- ------------------\n");

return 0;
}

