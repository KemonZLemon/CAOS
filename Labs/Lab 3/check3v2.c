/*
Expand your code to input a list of numbers of any length. Since we do not know
the length ahead of time, we will use dynamic memory (similar to Homework 1). Specifically,
start with an empty list; as each integer is read in via scanf(), call realloc() to extend the
length of the array by one element, adding the new number to the end of the array. Note
that you must keep track of the size of this array.
When the user is done entering in values, they will use CTRL-D. Your calls to fork() and
waitpid() should be as they were above. One approach for the child process is to make a
copy of the original array, sort this copy in non-descending order, then see how many numbers
were initially in the correct position.
To sort the array of elements, you could implement a sorting algorithm directly or you could
use the qsort() function. See the man page for qsort() for details and an example.
Below are some example program executions.
bash$ ./a.out
Enter a list of integers in non-descending order (CTRL-D to end):
1 2 3 4 5 6
<CTRL-D>
CHILD: correct order is 1 2 3 4 5 6
PARENT: child process reported 6 in the correct position
bash$ ./a.out
Enter a list of integers in non-descending order (CTRL-D to end):
2 2 2 3 3 2
<CTRL-D>
CHILD: correct order is 2 2 2 2 3 3
PARENT: child process reported 4 in the correct position
bash$
What is the largest valid number that the child process can return as its exit status to the
parent process?
*/

#include <stdio.h>
#include <stdlib.h>
#include<unistd.h>
#include<sys/wait.h>

void swap(int *a, int *b) {
  int temp = *a;
  *a = *b;
  *b = temp;
}

int main() {
  int n1, n2, n3;

  printf("Enter three integers in non-descending order: ");
  if (scanf("%d %d %d", &n1, &n2, &n3) != 3) {
    fprintf(stderr, "incorrect input(s)\n");
    return EXIT_FAILURE;
  }

  pid_t p = fork();

  if (p < 0) {
    perror("fork() failed");
    return EXIT_FAILURE;
  }
  //child process
  if (p == 0) {
    int n1copy = n1;
    int n2copy = n2;
    int n3copy = n3;

    if (n1 > n2) {
      swap(&n1, &n2);
    }
    if (n2 > n3) {
      swap(&n2, &n3);
    }
    if (n1 > n2) {
      swap(&n1, &n2);
    }


    printf("CHILD: correct order is %d %d %d\n", n1, n2, n3);
    int correctcount = 0;
    if (n1copy == n1) {
      correctcount++;
    }
    if (n2copy == n2) {
      correctcount++;
    }
    if (n3copy == n3) {
      correctcount++;
    }
    exit(correctcount);
  }
  //parent
  else {
    int status;

    waitpid(p, &status, 0);
    //if terminated normally
    if (WIFEXITED(status)) {
      //if WIFEXITED is true get exit code
      int child_status = WEXITSTATUS(status);
      printf("PARENT: child process reported %d in the correct position\n", child_status);
      //terminated abnormally
    } else if (WIFSIGNALED(status)) {
      //if WIFSIGNALED is true get exit code
      int signal = WTERMSIG(status);
      printf("PARENT: child process was terminated by signal %d\n", signal);
    }
  }
  return EXIT_SUCCESS;
}