#include <lexer.h>

struct lexer {
  struct sourcefile *sourcefile;
  char *code;
  size_t pos;
  size_t len;
  int row;
  int col;
  struct vec *toks;
};

static void readid(struct lexer *);
static void readnum(struct lexer *);
static void readstr(struct lexer *, char);
static void whitespace(struct lexer *);
static int peekc(struct lexer *);
static int peeknc(struct lexer *);
static int readc(struct lexer *);
static void addtok(struct lexer *, toktype_t, char *val);

struct tok *tok_new(toktype_t type, char *val, struct sourcepos *sourcepos) {
  struct tok *tok = malloc(sizeof(struct tok));
  tok->type = type;
  tok->val = val;
  tok->sourcepos = sourcepos_inc_ref(sourcepos);

  return tok;
}

void tok_free(struct tok *tok) {
  sourcepos_dec_ref(tok->sourcepos);
  free(tok->val);
  free(tok);
}

struct vec *lexer_tokenize(struct sourcefile *sourcefile) {
  struct lexer lexer;
  lexer.sourcefile = sourcefile;
  lexer.code = sourcefile->contents;
  lexer.pos = 0;
  lexer.len = strlen(sourcefile->contents);
  lexer.toks = vec_new();
  lexer.row = 0;
  lexer.col = 0;

  while (lexer.pos < lexer.len) {
    whitespace(&lexer);
    char cur = (char)peekc(&lexer);
    int next = peeknc(&lexer);
    if (cur == -1) break;

    if (cur == '"' || cur == '\'') {
      readstr(&lexer, cur);
    } else if (isalpha(cur) || cur == '_') {
      readid(&lexer);
    } else if (isdigit(cur)) {
      readnum(&lexer);
    } else {
      switch (cur) {
        case '+':
        case '-':
          if (next == '=') {
            addtok(&lexer, TOK_ASSIGN, heap_str(2, cur, '='));
            readc(&lexer);
          } else if (next == cur) {
            addtok(&lexer, TOK_OP, heap_str(2, cur, next));
            readc(&lexer);
          } else {
            addtok(&lexer, TOK_OP, heap_str(1, cur));
          }
          break;
        case '*':
        case '/':
        case '%':
          if (next == '=') {
            addtok(&lexer, TOK_ASSIGN, heap_str(2, cur, '='));
            readc(&lexer);
          } else {
            addtok(&lexer, TOK_OP, heap_str(1, cur));
          }
          break;
        case '=':
          if (next == '=') {
            addtok(&lexer, TOK_OP, heap_str(2, cur, '='));
            readc(&lexer);
          } else {
            addtok(&lexer, TOK_ASSIGN, heap_str(1, cur));
          }
          break;
        case '>':
        case '<':
          if (next == '=') {
            addtok(&lexer, TOK_OP, heap_str(2, cur, '='));
            readc(&lexer);
          } else {
            addtok(&lexer, TOK_OP, heap_str(1, cur));
          }
          break;
        case '!':
          if (next == '=') {
            addtok(&lexer, TOK_OP, heap_str(2, cur, '='));
            readc(&lexer);
          } else {
            addtok(&lexer, TOK_OP, heap_str(1, cur));
          }
          break;
        case '.':
          addtok(&lexer, TOK_DOT, heap_str(1, cur));
          break;
        case ',':
          addtok(&lexer, TOK_COMMA, heap_str(1, cur));
          break;
        case ':':
          addtok(&lexer, TOK_COLON, heap_str(1, cur));
          break;
        case ';':
          addtok(&lexer, TOK_SEMICOLON, heap_str(1, cur));
          break;
        case '[':
          addtok(&lexer, TOK_OSQUARE, heap_str(1, cur));
          break;
        case ']':
          addtok(&lexer, TOK_CSQUARE, heap_str(1, cur));
          break;
        case '{':
          addtok(&lexer, TOK_OBRACE, heap_str(1, cur));
          break;
        case '}':
          addtok(&lexer, TOK_CBRACE, heap_str(1, cur));
          break;
        case '(':
          addtok(&lexer, TOK_OPAREN, heap_str(1, cur));
          break;
        case ')':
          addtok(&lexer, TOK_CPAREN, heap_str(1, cur));
          break;
        default:
          printf("Unknown char '%c'!\n", cur);
          break;
      }
      readc(&lexer);
    }
  }

  struct vec *toks = lexer.toks;
  return toks;
}

static void readid(struct lexer *lexer) {
  struct strbuf *strbuf = strbuf_new();
  while (peekc(lexer) != -1 &&
         (isalnum((char)peekc(lexer)) || (char)peekc(lexer) == '_'))
    strbuf_append(strbuf, (char)readc(lexer));
  char *str = strbuf_done(strbuf);
  if (strcmp(str, "is") == 0) {
    addtok(lexer, TOK_OP, str);
  } else {
    addtok(lexer, TOK_ID, str);
  }
}

static void readnum(struct lexer *lexer) {
  struct strbuf *strbuf = strbuf_new();
  bool has_dot = false;
  while (peekc(lexer) != -1 && (isdigit((char)peekc(lexer)) ||
                                (!has_dot && (char)peekc(lexer) == '.')))
    strbuf_append(strbuf, (char)readc(lexer));
  addtok(lexer, TOK_NUM, strbuf_done(strbuf));
}

static void readstr(struct lexer *lexer, char delin) {
  struct strbuf *strbuf = strbuf_new();
  readc(lexer);
  while (peekc(lexer) != -1 && (char)peekc(lexer) != delin)
    strbuf_append(strbuf, (char)readc(lexer));
  readc(lexer);
  char *val = strbuf_done(strbuf);
  addtok(lexer, TOK_STRING, val);
}

static void whitespace(struct lexer *lexer) {
  while (peekc(lexer) != -1 && isspace((char)peekc(lexer))) readc(lexer);
}

static int peekc(struct lexer *lexer) {
  if (lexer->pos >= lexer->len) return -1;
  return lexer->code[lexer->pos];
}

static int peeknc(struct lexer *lexer) {
  if (lexer->pos + 1 >= lexer->len) return -1;
  return lexer->code[lexer->pos + 1];
}

static int readc(struct lexer *lexer) {
  if (lexer->pos >= lexer->len) return -1;
  char ret = lexer->code[lexer->pos++];

  if (ret == '\n') {
    lexer->row++;
    lexer->col = 0;
  } else {
    lexer->col++;
  }

  return ret;
}

static void addtok(struct lexer *lexer, toktype_t type, char *val) {
  int col_offset = val != NULL ? strlen(val) : 0;
  vec_push(lexer->toks,
           tok_new(type, val,
                   sourcepos_new(lexer->row, lexer->col - col_offset,
                                 lexer->sourcefile)));
}

void free_toks(struct vec *toks) {
  for (int i = 0; i < toks->len; ++i) {
    tok_free(vec_get(toks, i));
  }
  vec_free(toks);
}

void debug_toks(struct vec *toks) {
  for (int i = 0; i < toks->len; ++i) {
    struct tok *tok = vec_get(toks, i);
    printf("Type: %d, Value: %s, Row: %d, Col: %d\n", tok->type, tok->val,
           tok->sourcepos->row + 1, tok->sourcepos->col + 1);
  }
}