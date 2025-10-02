/* lab05-checkpoint3.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>

#define MAXWORDLENGTH 32

typedef struct trie
{
  char * word;        /* NULL if no valid word; otherwise, ptr to valid word */
  struct trie * next; /* ptr to next level of trie; or NULL if no next level */
}
trie_t;


/* Display all words in the given Trie */
void printtrie( const trie_t * trie )
{
  if ( trie == NULL ) return;  /* safeguard against NULL pointer */

  const trie_t * ptr = trie;

  /* visit each of the 26 elements of this layer of the Trie */
  for ( int i = 0 ; i < 26 ; i++ )
  {
    /* display valid word, if present */
    if ( (*(ptr + i)).word ) printf( "%s\n", (*(ptr + i)).word );

    /* recurse to the next level, if present */
    if ( (*(ptr + i)).next ) printtrie( (*(ptr + i)).next );
  }
}


/* convert the given word to UPPERCASE */
void upcaseword( char * word, int wordlen )
{
  for ( int i = 0 ; i < wordlen ; i++ )
  {
    word[i] = toupper( (unsigned char) word[i] );
  }
}


/* map uppercase letter 'A'-'Z' to index 0-25 */
int trieindex( char letter )
{
  return letter - 'A';
}


/* given a word in UPPERCASE, determine if it is valid */
int isvalidword( const trie_t * trie, const char * word, int wordlen )
{
  const trie_t * current = trie;
  for ( int i = 0 ; i < wordlen ; i++ )
  {
    int idx = trieindex( word[i] );
    if ( idx < 0 || idx >= 26 ) return 0;
    if ( current[idx].next == NULL && i < wordlen - 1 ) return 0;
    if ( i < wordlen - 1 )
    {
      current = current[idx].next;
    }
    else
    {
      /* last character: must have a word stored here */
      return ( current[idx].word != NULL );
    }
  }
  return 0;
}


/* free all dynamically allocated memory in the trie */
void freetrie( trie_t * trie )
{
  if ( trie == NULL ) return;

  for ( int i = 0 ; i < 26 ; i++ )
  {
    if ( trie[i].word )
    {
      free( trie[i].word );
      trie[i].word = NULL;
    }
    if ( trie[i].next )
    {
      freetrie( trie[i].next );
      free( trie[i].next );
      trie[i].next = NULL;
    }
  }
}


/* add the given word to the Trie */
void addword( trie_t * trie, const char * word, int wordlen )
{
  trie_t * current = trie;

  for ( int i = 0 ; i < wordlen ; i++ )
  {
    int idx = trieindex( word[i] );
    if ( idx < 0 || idx >= 26 ) return;  /* ignore invalid characters */

    if ( i == wordlen - 1 )
    {
      /* last character: store word here */
      if ( current[idx].word == NULL )
      {
        current[idx].word = malloc( (wordlen + 1) * sizeof(char) );
        if ( current[idx].word == NULL )
        {
          perror( "malloc() failed" );
          exit( EXIT_FAILURE );
        }
        strcpy( current[idx].word, word );
      }
    }
    else
    {
      /* not last character: go deeper */
      if ( current[idx].next == NULL )
      {
        current[idx].next = calloc( 26, sizeof(trie_t) );
        if ( current[idx].next == NULL )
        {
          perror( "calloc() failed" );
          exit( EXIT_FAILURE );
        }
      }
      current = current[idx].next;
    }
  }
}


int main( int argc, char ** argv )
{
  /* create array to hold top layer of the Trie */
  trie_t * trie = calloc( 26, sizeof( trie_t ) );
  if ( trie == NULL ) { perror( "calloc() failed" ); return EXIT_FAILURE; }

  /* add words to the Trie; modify/expand as you like */
  addword( trie, "AA", 2 );
  addword( trie, "AAH", 3 );
  addword( trie, "AAHED", 5 );
  addword( trie, "AAHING", 6 );
  addword( trie, "AAHS", 4 );
  addword( trie, "AAL", 3 );
  addword( trie, "AALII", 5 );
  addword( trie, "AALIIS", 6 );
  addword( trie, "AALS", 4 );
  addword( trie, "AARDVARK", 8 );
  addword( trie, "CAOS", 4 );

  printf( "Trie contains valid words:\n" );
  printtrie( trie );

  /* loop to repeatedly ask the user to input a word to look up */
  while ( 1 )
  {
    char buffer[33];
    char * alternate = NULL;
    printf( "Enter word to look up (CTRL-D to exit): " );
    if ( scanf( "%32s", buffer ) != 1 ) break;
    upcaseword( buffer, strlen( buffer ) );
    int rc = isvalidword( trie, buffer, strlen( buffer ) );
    printf( "==> %s is %s", buffer, rc == 1 ? "valid" : "not valid" );
    /* suggestword not implemented in this checkpoint */
    if ( rc == 0 && alternate ) printf( " (%s)", alternate );
    printf( "\n" );
  }

  /* free up all dynamically allocated memory for the given Trie */
  freetrie( trie );
  free( trie );

  return EXIT_SUCCESS;
}
