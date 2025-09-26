#include <stdio.h>
#include <stdlib.h>

int main(void) {
  FILE *fp;            // A "file pointer" (used to access the file)
  char *buffer;        // A pointer to hold our text
  int ch;              // Will hold each character from the file
  long size = 0;       // Keep track of how many characters
  long capacity = 1024; // Start with space for 1024 characters

  // Open the file for reading
  fp = fopen("lab04-data-W2.dat", "r");
  if (fp == NULL) {
    printf("Could not open file!\n");
    return 1;
  }

  // Allocate some memory to start
  buffer = (char *)malloc(capacity);
  if (buffer == NULL) {
    printf("Memory allocation failed!\n");
    fclose(fp);
    return 1;
  }

  // Read characters one at a time
  while ((ch = fgetc(fp)) != EOF) {
    // If we run out of space, double the capacity
    if (size >= capacity - 1) {
      capacity = capacity * 2;
      buffer = (char *)realloc(buffer, capacity);
      if (buffer == NULL) {
        printf("Memory reallocation failed!\n");
        fclose(fp);
        return 1;
      }
    }

    // Store the character
    *(buffer + size) = (char)ch;
    size++;
  }


  *(buffer + size) = '\0';

  printf("%s\n", buffer);


  free(buffer);
  fclose(fp);

  return 0;
}
