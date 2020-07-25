#include <compiler/token.h>

struct token * token_init (tok_type_t type, char * str) {
  struct token * t;

  t       = (struct token *)calloc (1, sizeof (struct token));
  t->type = type;
  t->str  = str;

  return t;
}

void token_free (struct token * t) {
  if (t->str) {
    free (t->str);
  }

  free (t);
}
