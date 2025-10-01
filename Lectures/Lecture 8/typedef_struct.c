/* typedef-struct.c */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* typedef ... <type-name> */

typedef struct
{
  unsigned int rin;   /* e.g., 660000001 */
  char rcsid[20];     /* e.g., "goldsd3" */
  /* ... */
}
student_t;

int main()
{
  printf( "sizeof student_t is %lu\n", sizeof( student_t ) );
  /* see: sizeof.c example... */

  /* static allocation */
  student_t me;
  me.rin = 660000001;
  strcpy( me.rcsid, "goldsd3" );

  /* dynamic allocation */
  student_t * st;
  st = malloc( sizeof( student_t ) );   /* or calloc() */
  (*st).rin = 661234567;    /* st->rin = 661234567; */
  strcpy( (*st).rcsid, "smithj123" );

  printf( "me: RIN %u; RCSID %s\n", me.rin, me.rcsid );
  printf( "st: RIN %u; RCSID %s\n", (*st).rin, (*st).rcsid );
  printf( "st: RIN %u; RCSID %s\n", st->rin, st->rcsid );

  free( st );

  /***********************************************************/

  /* dynamically allocate an array of student_t of size 7000 */
  student_t * students = calloc( 7000, sizeof( student_t ) );

  /* this is a "shallow" copy...
   * ...which means that the assignment below only copies the
   *     static allocations/variables
   */
  students[500] = me;
  *(students + 500) = me;

  printf( "students[500]: RIN %u; RCSID %s\n",
         students[500].rin, students[500].rcsid );
  printf( "students[500]: RIN %u; RCSID %s\n",
         (*(students + 500)).rin, (*(students + 500)).rcsid );
  printf( "students[500]: RIN %u; RCSID %s\n",
         (students + 500)->rin, (students + 500)->rcsid );

  free( students );

  return EXIT_SUCCESS;
}