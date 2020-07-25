#include <compiler/lexer.h>

static void lexer_read_id    (struct lexer * l);
static void lexer_read_float (struct lexer * l);
static void lexer_read_str   (struct lexer * l);

static void lexer_eat_wspace (struct lexer * l);
static void lexer_rw         (struct lexer * l, int count);
static void lexer_ff         (struct lexer * l, int count);
static int  lexer_peekc      (struct lexer * l);
static int  lexer_readc      (struct lexer * l);

struct lexer * lexer_init (char * src) {
  struct lexer * l;

  l       = (struct lexer *)calloc (1, sizeof (struct lexer));
  l->src  = src;
  l->pos  = 0;
  l->len  = strlen (src);
  l->toks = vector_init ();

  return l;
}

void lexer_free (struct lexer * l) {
  if (l->toks) {
    vector_deep_free (l->toks, free);
  }

  free (l);
}

void lexer_run (struct lexer * l) {

}

static void lexer_read_id (struct lexer * l) {
  int    id_str_len;
  char * str;

  id_str_len = 0; // account for \0
  while (isalnum ((char) lexer_peekc (l))) {
    id_str_len ++;
    lexer_readc (l);
  }
  lexer_rw (l, id_str_len);

  str = (char *)calloc (1, sizeof (char) * (id_str_len + 1));
  strncpy (str, l->src + l->pos, id_str_len);
  str [id_str_len] = '\0';
  lexer_ff (l, id_str_len);

  vector_push (l->toks, token_init (id_tok, str));
}

static void lexer_eat_wspace (struct lexer * l) {
  while (iswspace ((char)lexer_peekc (l))) {
    lexer_readc (l);
  }
}

static void lexer_ff (struct lexer * l, int count) {
  if ((l->pos - count) < 0 || (l->pos + count) >= l->len) {
    printf ("INTERNAL LEXER ERROR: lexer_ff was passed %d when len is %d and pos is %d!\n", count, l->len, l->pos);
    exit   (-1);
  }

  l->pos += count;
}

static void lexer_rw (struct lexer * l, int count) {
  if ((l->pos - count) < 0 || (l->pos + count) >= l->len) {
    printf ("INTERNAL LEXER ERROR: lexer_rw was passed %d when len is %d and pos is %d!\n", count, l->len, l->pos);
    exit   (-1);
  }

  l->pos -= count;
}

static int lexer_peekc (struct lexer * l) {
  return l->pos < l->len ? l->src [l->pos] : -1;
}

static int lexer_readc (struct lexer * l) {
  return l->pos < l->len ? l->src [l->pos ++] : -1;
}
