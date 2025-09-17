//
// Created by zhuke on 9/16/2025.
//

/*
Write a C program that reads and processes any number of input files,
counting the number of uppercase letters, lowercase letters, digits, punctuation, as well as
the number of lines. The filenames are given as command-line arguments.
You are required to use open(), read(), and close() for this checkpoint. As you process
each file, if any errors occur, display a meaningful error message, then attempt to move on
to the next file. In other words, do not just exit the program.
Reading one byte at a time from each file is inefficient. Therefore, make use of a temporary
buffer to read in “chunks” of data from each file. Mimic the fd-read.c example. Be sure to
carefully read the man page for read(), in particular the return value—there are three cases
to handle.
For your counts, make use of the isupper(), islower(), isdigit(), and ispunct() library
functions. Below are some example program executions.
bash$ ./a.out
USAGE: ./a.out <filename> ...
bash$ ./a.out lab03-checkpoint1.c nosuchfile.txt testfile.txt
Processing "lab03-checkpoint1.c"...
Processing "nosuchfile.txt"...
open() failed: No such file or directory
Processing "testfile.txt"...
SUMMARY AFTER PROCESSING 61 LINES IN 2 FILES:
==> uppercase characters: 114
==> lowercase characters: 719
==> digit characters: 18
==> punctuation characters: 327
bash$
Try using ls, wc, and other commands to help verify your results.
 */

/*
read() Return Values:
> 0: A positive value indicates the number of bytes successfully read into the buffer. This number can be less than the requested buffer size,
especially near the end of the file.

0: A return value of zero indicates that the end of the file (EOF) has been reached. No more data can be read.

-1: A negative one indicates that an error occurred. The specific error is stored in the global variable errno.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

int main(int argc, char * argv[])
{
  int total_upper = 0, total_lower = 0, total_digit = 0, total_punct = 0, total_lines = 0, files = 0;
  //check command line
  if(argc < 2){
    fprintf(stderr, "USAGE: %s <filename> ... \n", argv[0]);
    return EXIT_FAILURE;
  }

  //opening files
  for(int i = 1; i < argc; i++){
    char *filename = argv[i];
    printf("Processing \"%s\"...\n", filename);
    //files;

    int fd = open(filename, O_RDONLY);
    if ( fd == -1 ) {
      perror( "open() failed" );
      continue;
    }

    char buffer[32];
    int rc;

    //read files
    //read returns positive
    while((rc = read(fd, buffer, sizeof(buffer))) > 0){
      for(int j = 0; j < rc; j++){
        char c = buffer[j];
        if(isupper(c)){
          total_upper++;
        }else if(islower(c)){
          total_lower++;
        }else if(isdigit(c)){
          total_digit++;
        }else if(ispunct(c)){
          total_punct++;
        }
        if( c == '\n'){
          total_lines++;
        }
      }
    }

    //read returns negative
    if(rc == -1){
      perror( "read() failed" );
      close(fd);
      continue;
    }


    //read returns 0
    close(fd);
  }
  printf("\nSUMMARY AFTER PROCESSING %d LINES IN %d FILES:\n", total_lines, files);
  printf("> uppercase characters: %d\n", total_upper);
  printf("> lowercase characters: %d\n", total_lower);
  printf("> digit characters: %d\n", total_digit);
  printf("> punctuation characters: %d\n", total_punct);

  return EXIT_SUCCESS;
}

