#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

/* A "file" is simply a stream of bytes --- read and/or write
 *
 * file descriptor(fd)
 *
 * --a small non-negative integer used in a variety of system calls
 * to refer to an open file (i.e., file stream of byte stream)
 *
 * --each process has a file descriptor table associated with it that
 * keeps track of its open (file) descriptors
 *
 */

int main() {
 char * name = "testfile.txt";

 int fd = open(name, O_RDONLY);
 if(fd == -1) {
  perror("Open failed");
  return EXIT_FAILURE;
 }

 printf("fd is %d\n", fd);
 char buffer[32];
 int rc = read(fd, buffer, 16);
 buffer[rc] = '\0';
 printf("read() returned  %d -- buffer \"%s\"\n", rc, buffer);

 rc = read(fd, buffer, 16);
 buffer[rc] = '\0';
 printf("read() returned  %d -- buffer \"%s\"\n", rc, buffer);

 rc = read(fd, buffer, 16);
 buffer[rc] = '\0';
 printf("read() returned  %d -- buffer \"%s\"\n", rc, buffer);

 rc = read(fd, buffer, 16);
 buffer[rc] = '\0';
 printf("read() returned  %d -- buffer \"%s\"\n", rc, buffer);

 while(1) {
  rc = read(fd, buffer, 16);
  printf("read() returned  %d -- buffer \"%s\"\n", rc, buffer);
 }

 close(fd);
return EXIT_SUCCESS;
}
