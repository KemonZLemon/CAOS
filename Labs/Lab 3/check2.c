/*
Write a C program that reads the file specified on the command line. Note
that only one file is expected.
Read the file in chunks just as you did in Checkpoint 1 above, but as you process each byte,
display both the character and the binary digits of the byte. If you detect a newline '\n'
character, display a '\' character followed by a 'n' character. And if the character is not
printable, which you can determine via isprint(), display "<non-printable>".
To display each byte in binary, write a reusable function called print_byte() that takes as
input an unsigned char and displays '0' and '1' characters to display the value in base 2.
Below are some example program executions.
bash$ ./a.out
USAGE: ./a.out <filename>
bash$ ./a.out lab03-checkpoint2.c
char '/' ==> byte 00101111
char '*' ==> byte 00101010
char ' ' ==> byte 00100000
char 'l' ==> byte 01101100
char 'a' ==> byte 01100001
char 'b' ==> byte 01100010
char '0' ==> byte 00110000
char '3' ==> byte 00110011
...
char 'S' ==> byte 01010011
char 'U' ==> byte 01010101
char 'C' ==> byte 01000011
char 'C' ==> byte 01000011
char 'E' ==> byte 01000101
char 'S' ==> byte 01010011
char 'S' ==> byte 01010011
char ';' ==> byte 00111011
char '\n' ==> byte 00001010
char '}' ==> byte 01111101
char '\n' ==> byte 00001010
bash$
Remember that you can find the ASCII table using man ascii or at https://asciitable.com.
And for non-printable characters, consider using an image file or an executable (binary) file.
An example using the a.out executable is shown below.
bash$ ./a.out a.out
char <non-printable> ==> byte 01111111
char 'E' ==> byte 01000101
char 'L' ==> byte 01001100
char 'F' ==> byte 01000110
char <non-printable> ==> byte 00000010
char <non-printable> ==> byte 00000001
...
Try using hexdump -C to view the contents of binary files.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

void print_byte(unsigned char byte) {
    int binary_digits[8];
    for (int i = 7; i >= 0; i--) {
        binary_digits[i] = byte % 2;
        // shift bit to right
        byte = byte / 2;
    }
    // print bits
    for (int i = 0; i < 8; i++) {
        printf("%d", binary_digits[i]);
    }
}

int main(int argc, char* argv[]) {
  if(argc != 2) {
    printf("Usage: %s <file_name>\n", argv[0]);
    return EXIT_FAILURE;
  }
  const char *filename = argv[1];
  int fd = open(filename, O_RDONLY);

  if (fd == -1) {
    perror("open() failed");
    return EXIT_FAILURE;
  }

  char buffer[4096];
  int rc;

  while((rc = read(fd, buffer, sizeof(buffer))) > 0){
    for(int j = 0; j < rc; j++){
      char current = buffer[j];
      if(current == '\r'){
        continue;
      }
      char display[10];

      if(current == '\n'){
        sprintf(display, "'\\n'");
      } else if(isprint(current)){
        sprintf(display, "'%c'", current);
      } else{
        sprintf(display, "\" \"");
      }
      printf("char %-5s == > byte ", display);
      print_byte(current);
      printf("\n");
    }
  }
  if(rc == -1){
    perror("read() failed");
    close(fd);
    return EXIT_FAILURE;
  }
  close(fd);
  return EXIT_SUCCESS;
}