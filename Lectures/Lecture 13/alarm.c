/* alarm.c */

/* Users can enter a timer amount (in seconds) and
 *  a string to display once the timer expires
 *
 * ==> 3 wake up
 * ALARM: wake up
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAXLINE 80

int main()
{
  char * line = calloc( MAXLINE + 1, sizeof( char ) );
  char * msg = calloc( MAXLINE + 1, sizeof( char ) );

  while ( 1 )
  {
    int seconds;

    printf( "Set alarm (<sec> <msg>) -- 0 to exit: " );

    if ( fgets( line, MAXLINE + 1, stdin ) == NULL ) break;

    if ( strlen( line ) <= 1 ) continue;  /* skip blank lines */

    /* TO DO: what happens when the user inputs more than 80 characters? */
#if 0
    if ( sscanf( line, "%d %s", &seconds, msg ) < 2 || seconds < 0 )
    if ( sscanf( line, "%d %[^\n0-9]", &seconds, msg ) < 2 || seconds < 0 )
    if ( sscanf( line, "%d %[A-Za-z ]", &seconds, msg ) < 2 || seconds < 0 )
#else
    /* %[^\n] will read all characters until the end of the given line */
    if ( sscanf( line, "%d %[^\n]", &seconds, msg ) < 2 || seconds < 0 )
#endif
    {
      if ( seconds == 0 ) break;
      fprintf( stderr, "ERROR: invalid alarm request\n" );
    }
    else
    {
      printf( "Alarm set for %d seconds\n", seconds );
      sleep( seconds );
      fprintf( stderr, "ALARM (%d): %s\n", seconds, msg );
      *msg = '\0';
    }
  }

  free( msg );
  free( line );

  return EXIT_SUCCESS;
}
