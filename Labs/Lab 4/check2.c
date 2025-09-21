#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <sys/wait.h>

void process_file(const char *filename) {
  pid_t my_pid = getpid();
  printf("CHILD %d: Processing \"%s\"...\n", my_pid, filename);

  int fd = open(filename, O_RDONLY);
  if (fd == -1) {
    fprintf(stderr, "CHILD %d: ", my_pid);
    perror("open() failed");
    exit(EXIT_FAILURE);
  }

  int upper_count = 0, lower_count = 0, digit_count = 0, punct_count = 0, line_count = 0;

  char buffer[32];
  int bytes_read;

  while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0) {
    for (int i = 0; i < bytes_read; i++) {
      char c = buffer[i];
      if (isupper(c)) {
        upper_count++;
      } else if (islower(c)) {
        lower_count++;
      } else if (isdigit(c)) {
        digit_count++;
      } else if (ispunct(c)) {
        punct_count++;
      }
      if (c == '\n') {
        line_count++;
      }
    }
  }

  if (bytes_read == -1) {
    fprintf(stderr, "CHILD %d: ", my_pid);
    perror("read() failed");
    close(fd);
    exit(EXIT_FAILURE);
  }

  close(fd);

  printf("CHILD %d: Lines: %d\n", my_pid, line_count);
  printf("CHILD %d: Uppercase characters: %d\n", my_pid, upper_count);
  printf("CHILD %d: Lowercase characters: %d\n", my_pid, lower_count);
  printf("CHILD %d: Digit characters: %d\n", my_pid, digit_count);
  printf("CHILD %d: Punctuation characters: %d\n", my_pid, punct_count);

  exit(EXIT_SUCCESS);
}

int main(int argc, char * argv[])
{
   if (argc < 2) {
     fprintf(stderr, "Usage: %s <filename>\n", argv[0]);
     return EXIT_FAILURE;
   }

  for (int i = 1; i < argc; i++) {
    pid_t p = fork();

    if (p < 0) {
      perror("fork() failed");
      return EXIT_FAILURE;
    } else if (p == 0) {
      process_file(argv[i]);
    }
  }

  pid_t parent = getpid();
  int status;
  pid_t child;

  for (int i = 1; i < argc; i++) {
    child = wait(&status);
    if (child == (pid_t) -1) {
      perror("wait() failed");
      return EXIT_FAILURE;
    }
    if (WIFEXITED(status)) {
      int exit_status = WEXITSTATUS(status);
      printf("PARENT %d: Child process %d terminated with exit status %d\n", parent, child, exit_status);
    } else {
      printf("PARENT %d: Child process %d terminated abnormally\n", parent, child);
    }
  }

  return EXIT_SUCCESS;
}

