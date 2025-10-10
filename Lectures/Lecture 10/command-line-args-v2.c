/* command-line-args-v2.c */

#include <stdio.h>
#include <stdlib.h>
                 /* char * argv[] */
int main( int argc, char ** argv )
{
  printf( "argc is %d\n", argc );   /* argument count */

  if ( argc != 4 )
  {
    fprintf( stderr, "ERROR: Invalid arguments\n" );
    fprintf( stderr, "USAGE: a.out <filename> <x> <y>\n" );
    return EXIT_FAILURE;
  }

  printf( "argv[0] is %s\n", argv[0] );   /* argv + 0 */
  printf( "argv[1] is %s\n", argv[1] );   /* argv + 1 */
  printf( "argv[2] is %s\n", argv[2] );   /* argv + 2 */
  printf( "argv[3] is %s\n", argv[3] );   /* argv + 3 */
  printf( "argv[argc] is %s\n", argv[argc] );   /* always NULL */

  for ( char ** ptr = argv ; *ptr != NULL ; ptr++ )
  {
    printf( "next arg is %s\n", *ptr );
  }

  for ( char ** ptr = argv ; *ptr ; ptr++ ) printf( "next arg is %s\n", *ptr );

  return EXIT_SUCCESS;
}

#if 0
                 char *
                +------+
  argv ---> [0] |  =======> "./a.out"
                |      |
                +------+
  ptr ----> [1] |  =======> "a.txt"
                +------+
            [2] |  =======> "12"
                +------+
            [3] |  =======> "34"
                +------+
            [4] | NULL |
                +------+
#endif
