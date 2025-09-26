#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  unsigned int rin;
  char rcsid[20];
}
student_t;

int main() {
  printf("sizeof student_t is %lu\n", sizeof(student_t));

  /* static allocations*/
  student_t me;
  me.rin = 600000001;
  strcpy(me.rcsid, "goldsd3");

  /* dynamic allocations */
  student_t * st;
  st = malloc(sizeof(student_t));   //or calloc
  (*st).rin = 661234567;
  strcpy((*st).rcsid, "smithj123");

  printf("me: RIN %u RCSID %s\n", me.rin, me.rcsid);
  printf("me: RIN %u RCSID %s\n", st -> rin, st -> rcsid);

  free(st);
  /* dynamically alocate an array of student_t of size 7000 */
  student_t * students = calloc(7000, sizeof(student_t));

  students[500] = me;
  *(students + 500) = me;
  printf("students[500]: RIN %u RCSID %s\n", students[500].rin, students[500].rcsid);
  printf("students[500]: RIN %u RCSID %s\n", (*(students + 500)).rin, (*(students + 500)).rcsid);
  printf("students[500]: RIN %u RCSID %s\n", (students + 500) -> rin, (students + 500) -> rcsid);

  free(students);
  return EXIT_SUCCESS;
}