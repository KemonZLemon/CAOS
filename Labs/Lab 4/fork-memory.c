/* fork-memory-pointer.c */

/* Note that there is a bug in this code!
 * TO DO: use valgrind to find it and fix it...
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>

#define CHUNK_SIZE 4

void block_on_waitpid(pid_t pid);
void child_process(char * data, int i);

int main()
{
  char * data = calloc(27, sizeof(char));
  if (!data) { perror("calloc failed"); return EXIT_FAILURE; }

  strcpy(data, "abcdefghijklmnopqrstuvwxyz");

  /* keep track of how many child processes are created */
  int children = 0;

  /* index variable i indicates to the child process which
   *  chunk of data to convert to uppercase and display;
   *   each child process works on a substring of length CHUNK_SIZE
   */
  int data_len = strlen(data);
  for (int i = 0; i < data_len; i += CHUNK_SIZE)

  {
    /* create the next child process */
    pid_t p = fork();  /* i == 0, 4, 8, 12, 16, 20, 24 */
    if (p == -1) { perror("fork() failed"); return EXIT_FAILURE; }

    if (p == 0)  /* CHILD process executes here... */
    {
      child_process(data, i);  /* <=== pass-by-fork occurs here */
      return EXIT_SUCCESS;
    }

    children++;
    printf("PARENT: my new child process has PID %d\n", p);
#if 0
    block_on_waitpid(p);   /* run in a "no parallel" mode */
#endif
  }

#if 1
  /* run in "parallel" mode, i.e., run all child processes simultaneously */
  while (children > 0)
  {
    block_on_waitpid(-1);
    children--;
  }
#endif

  printf("PARENT: all child processes have terminated\n");
  printf("PARENT: data: \"%s\"\n", data);

  free(data);

  return EXIT_SUCCESS;
}

void child_process(char * data, int i)
{
  for (int j = 0; j < CHUNK_SIZE; j++)
  {
    if (*(data + i + j) && isalpha(*(data + i + j)))   /* also see islower() */
    {
      *(data + i + j) = toupper(*(data + i + j));
    }
  }
  printf("CHILD %d: data: \"%s\"\n", getpid(), data);
}

void block_on_waitpid(pid_t pid)
{
  /* wait (BLOCK) for child process pid to complete/terminate */
  int status;
  pid_t child_pid = waitpid(pid, &status, 0);  /* BLOCKING */

  printf("PARENT: child process %d terminated...\n", child_pid);

  if (WIFSIGNALED(status))
  {
    printf("PARENT: ...abnormally (killed by a signal)\n");
  }
  else if (WIFEXITED(status))
  {
    int exit_status = WEXITSTATUS(status);
    printf("PARENT: ...normally with exit status %d\n", exit_status);
  }
}
