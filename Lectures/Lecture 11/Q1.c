/* Q1.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

  int main()
  {
    int rc;
    printf( "ONE-%u\n", getpid() );
    rc = fork();
    printf( "TWO-%u\n", getpid() );
    if ( rc == 0 ) { printf( "THREE-%u\n", getpid() ); }
    if ( rc > 0 ) { printf( "FOUR-%u\n", getpid() ); }
    return EXIT_SUCCESS;
  }

#if 0
  bash$ ./a.out

                ONE-777
                 fork()
                /      \
      <parent> /        \ <child>
              /          \
     =========            =========
       TWO-777  <======>  TWO-800
       FOUR-777           THREE-800
     =========            =========

    The two blocks (pairs) of output lines could
     interleave with one another...
      ...because we are in a line-based buffering mode


  bash$ ./a.out > output.txt

                 fork()
                /      \
      <parent> /        \ <child>
              /          \
     =========            =========
   (stdout buffer)     (stdout buffer)
       ONE-777            ONE-777
       TWO-777            TWO-800
       FOUR-777           THREE-800
     =========            =========
   (flush the           (flush the
    stdout buffer)       stdout buffer)

   There is NO interleaving of the lines of output above

   Based on the buffering mode (fully/block buffered),
    there are TWO possible outputs for the output.txt
     file...






#endif

