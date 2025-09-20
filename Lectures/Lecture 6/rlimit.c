#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/resource.h>

int main() {
  struct rlimit rl;
  getrlimit(RLIMIT_NPROC, &rl);
  //soft and hard limit of processes you can create
  printf("RLIMIT_NPROC soft limit: %ld\n", rl.rlim_cur);
  printf("RLIMIT_NPROC hard limit: %ld\n", rl.rlim_max);

  //lower the RLIMIT_NPROC soft limit
  rl.rlim_cur = 16;
  setrlimit(RLIMIT_NPROC, &rl);
  printf("RLIMIT_NPROC soft limit: %ld\n", rl.rlim_cur);
  printf("RLIMIT_NPROC hard limit: %ld\n", rl.rlim_max);

  while (1) {
    pid_t p = fork();
    if (p == -1) {
      perror("fork() failed");
      return EXIT_FAILURE;
    }
    printf("PID %d: fork() worked\n", getpid());
    sleep(3); //delay/suspend the process for 3 seconds
  }

  return EXIT_SUCCESS;
}