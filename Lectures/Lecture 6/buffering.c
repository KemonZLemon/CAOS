#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
/*When printing to the terminal via stdout, output is buffered in
 *
 */


int main() {
  printf("how about here?!"); /* stdout buffer: "how about here?!" */
                                      /* (this buffer is in memory...) */
  int * x = NULL;

  printf("HERE0\nXYZ");  /* stdout buffer: how about here?!HEREO0\n
                                    the '\n' char causes the buffer to be flushed
                                    stdout buffer: "XYZ*/


  *x = 1234; //seg fault bc dereferencing a null pointer


  printf("*x is %d\n", *x);

  return EXIT_SUCCESS;
}