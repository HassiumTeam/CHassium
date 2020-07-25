#include <stdio.h>
#include <util/vector.h>

int main (int argc, char * argv []) {
  vector_t * v = vector_init ();

  int a = 4;
  int b = 7;
  int c = 19;

  vector_push (v, &a);
  vector_push (v, &b);
  vector_push (v, &c);
  vector_set (v, 5, &b);

  for (int i = 0; i < 3 ;i++) {
    printf ("%d \n", *(int *)vector_get (v, i));
  }

  printf ("%d\n", *(int *)vector_get (v, 5));

  vector_free (v);
}
