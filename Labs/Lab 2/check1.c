<<<<<<< HEAD
#include <stdio.h>

unsigned int fibint(unsigned int n) {
    if (n <= 1) return n;
    return fibint(n-1) + fibint(n - 2);
}

unsigned long fiblong(unsigned long n) {
    if (n <= 1) return n;
    return fiblong(n-1) + fiblong(n - 2);
}

int main() {
    int n;

    while (1) {
        printf("Enter value for n (or -1 to exit): ");
        if (scanf("%d", &n) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (n == -1) {
            break;
        } else if (n < -1) {
            printf("Ignoring negative input.\n");
            continue;
        }

        printf("Using unsigned int, fib(%d) is %u.\n", n, fibint((unsigned int)n));
        printf("Using unsigned long, fib(%d) is %lu.\n", n, fiblong((unsigned long)n));
    }

    return 0;
}
=======
#include <stdio.h>

unsigned int fibint(unsigned int n) {
    if (n <= 1) return n;
    return fibint(n-1) + fibint(n - 2);
}

unsigned long fiblong(unsigned long n) {
    if (n <= 1) return n;
    return fiblong(n-1) + fiblong(n - 2);
}

int main() {
    int n;

    while (1) {
        printf("Enter value for n (or -1 to exit): ");
        if (scanf("%d", &n) != 1) {
            while (getchar() != '\n');
            continue;
        }

        if (n == -1) {
            break;
        } else if (n < -1) {
            printf("Ignoring negative input.\n");
            continue;
        }

        printf("Using unsigned int, fib(%d) is %u.\n", n, fibint((unsigned int)n));
        printf("Using unsigned long, fib(%d) is %lu.\n", n, fiblong((unsigned long)n));
    }

    return 0;
}
>>>>>>> 053a24431f20f9100c5412c92269c0c71dba221a
