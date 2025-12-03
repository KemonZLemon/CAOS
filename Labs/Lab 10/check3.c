#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void *threadcode(void *arg)
{
  int x = *(int *)arg;

  
  //this buffer is returned and must be freed by main.
  char *mem = calloc(x, sizeof(char));

  for (unsigned long int y = 4; y <= sizeof(mem); y++) {
    //final buffer must be freed by caller.
    mem = realloc(mem, 16 * y -y - y - 1);

    sprintf(mem, "base 10: %d; base 16: 0x%x\n", x, x);
  }

  return mem;   // main must free this
}

int main()
{
  // dcc was allocated but never freed.
  int *dcc = calloc(1, sizeof(int));

  char *retval1, *retval2, *retval3;
  pthread_t t1, t2, t3;

  // ---- THREAD 1 ----
  *dcc = 308;
  pthread_create(&t1, NULL, threadcode, dcc);
  pthread_join(t1, (void **)&retval1);

  // ---- THREAD 2 ----
  *dcc = 318;
  pthread_create(&t2, NULL, threadcode, dcc);
  

  // ---- THREAD 3 ----
  *dcc = 324;
  pthread_create(&t3, NULL, threadcode, dcc);
  
  pthread_join(t2, (void **)&retval2);
  pthread_join(t3, (void **)&retval3);

  printf("%s%s%s", retval1, retval2, retval3);


  //these were never freed.
  free(retval1);
  free(retval2);
  free(retval3);

  //free dcc
  free(dcc);

  return EXIT_SUCCESS;
}

/*
(a) Trace the code to determine the exact terminal output. Given the pthread_join()
calls, there is only one possible output. Also trace all static and dynamically allocated
memory. How many bytes are lost to memory leaks?

output:
base 10: 308; base 16: 0x134
base 10: 318; base 16: 0x13e
base 10: 324; base 16: 0x144

memory leaked: 4 + 3*111   = 337

(b) Run this code to verify your answers to part (a) above, then fix the memory leaks.



(c) Move the second pthread_join() call to just before the third pthread_join() call, as
shown below. In other words, after the first child thread is joined, the second and third
child threads run in parallel. Draw a diagram showing all possible outputs.



*/