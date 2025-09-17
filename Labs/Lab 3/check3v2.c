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

int compare(const void *a, const void *b) {
  int x = *(int*)a;
  int y = *(int*)b;
  return x - y;
}

int main() {
  int * array = NULL;
  int size = 0;
  int num;

  printf("Enter three integers in non-descending order: ");

  while (scanf("%d", &num) != EOF) { //==1
    int * temp = realloc(array,(size+1) * (sizeof(int)));
    if (!temp) {
      free(array);
      perror("realloc() failed");
      return EXIT_FAILURE;
    }
    array = temp;
    array[size++] = num;
  }

  if (size == 0) {
    printf("No numbers entered");
    free(array);
    return EXIT_SUCCESS;
  }

  pid_t p = fork();

  if (p < 0) {
    perror("fork() failed");
    free(array);
    return EXIT_FAILURE;
  }
  //child process
  if (p == 0) {
    int * sorted = malloc(size * sizeof(int));
    if (!sorted) {
      perror("malloc() failed");
      return EXIT_FAILURE;
    }
    for (int i = 0; i < size; i++) {
      sorted[i] = array[i];
    }

    qsort(sorted, size, sizeof (int), compare);

    printf("CHILD: correct order is: ");
    for (int i = 0; i < size; i++) {
      printf("%d ", sorted[i]);
    }
    printf("\n");

    int correctcount = 0;
    for (int i = 0; i < size; i++) {
      if (sorted[i] == array[i]) {
        correctcount++;
      }
    }
    free(sorted);

    if (correctcount > 255) {
      correctcount = 255;
      exit(correctcount);
    }
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
  free(array);
  return EXIT_SUCCESS;
}