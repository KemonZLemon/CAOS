/*
Write a C program that asks the user to input three integers in nondescending order; use scanf() for this. Of course, the given integers may or may not be
in non-descending order.
Next, use fork() to create a new child process. This child process must display the numbers
in non-descending order. Also, the child process must determine how many integers were
already in the correct order.
For example, if the three values given were 22, 24, and 40, in that order, then the child
process exits with an “exit status” of 3. If instead the values given were 24, 13, and 22, then
the “exit status” is 0 since non-descending order is 13, 22, and 24—and none of the values
given was in the correct positions.
The parent process must wait until the child process completes this task and terminates
(i.e., the child process exits). Once the parent process detects this, the parent process must
report the “exit status” of the child process. If the child process was shut down by a signal,
the parent process must also report this.
Below are some example program executions.
bash$ ./a.out
Enter three integers in non-descending order: 1 2 three
ERROR: incorrect input(s)
bash$ ./a.out
Enter three integers in non-descending order: 22 24 40
CHILD: correct order is 22 24 40
PARENT: child process reported 3 in the correct position
bash$ ./a.out
Enter three integers in non-descending order: 22 24 24
CHILD: correct order is 22 24 24
PARENT: child process reported 3 in the correct position
bash$ ./a.out
Enter three integers in non-descending order: 24 13 22
CHILD: correct order is 13 22 24
PARENT: child process reported 0 in the correct position
bash$ ./a.out
Enter three integers in non-descending order: 24 24 22
CHILD: correct order is 22 24 24
PARENT: child process reported 1 in the correct position
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
