/* sizeof.c */

#include <stdio.h>
#include <stdlib.h>

/* Since we are not using most of the declared variables,
 *  you will need to ignore warnings when you compile...
 *   ...but just for this example!
 */

int main()
{
  /* add unsigned to any of these data types... */

  char c = '?';
  int x = 13;
  short s = 2;
  long l = 123456789123456789;

  float f = 3.14159;
  double lf = 3.14159;

  /* TO DO: practice using printf() to display these variables...
   *        ...figure out how rounding occurs for float/double
   *        ...what about integer overflow/underflow
   */

  printf( "sizeof char is %lu\n", sizeof( char ) );
  printf( "sizeof c is %lu\n", sizeof( c ) );

  printf( "sizeof int is %lu\n", sizeof( int ) );
  printf( "sizeof short is %lu\n", sizeof( short ) );
  printf( "sizeof long is %lu\n", sizeof( long ) );
  printf( "sizeof float is %lu\n", sizeof( float ) );
  printf( "sizeof double is %lu\n", sizeof( double ) );

  printf( "\n" );

  /* (we are running on a 64-bit architecture) */
  printf( "sizeof void* is %lu\n", sizeof( void* ) );
  printf( "sizeof int* is %lu\n", sizeof( int* ) );
  printf( "sizeof short* is %lu\n", sizeof( short* ) );
  printf( "sizeof long* is %lu\n", sizeof( long* ) );
  printf( "sizeof char* is %lu\n", sizeof( char* ) );
  printf( "sizeof float* is %lu\n", sizeof( float* ) );
  printf( "sizeof double* is %lu\n", sizeof( double* ) );

  printf( "\n" );

  return EXIT_SUCCESS;
}
