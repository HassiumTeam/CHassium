#include <lexer.h>

struct lexer {
  char *code;
  size_t pos;
  size_t len;
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

struct tok *tok_new(toktype_t type, char *val) {
  struct tok *tok = (struct tok *)calloc(1, sizeof(struct tok));
  tok->type = type;
  tok->val = val;
  return tok;
}

void tok_free(struct tok *tok) {
  free(tok->val);
  free(tok);
}

struct vec *lexer_tokenize(char *code) {
  struct lexer lexer;
  lexer.code = code;
  lexer.pos = 0;
  lexer.len = strlen(code);
  lexer.toks = vec_new();

  while (lexer.pos < lexer.len) {
    whitespace(&lexer);
    char cur = (char)peekc(&lexer);
    int next = peeknc(&lexer);
    if (cur == -1) break;

    if (cur == '"' || cur == '\'')
      readstr(&lexer, cur);
    else if (isalpha(cur) || cur == '_')
      readid(&lexer);
    else if (isdigit(cur))
      readnum(&lexer);
    else {
      switch (cur) {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
          if (next == '=') {
            addtok(&lexer, TOK_ASSIGN, heap_str(2, cur, '='));
            readc(&lexer);
          } else
            addtok(&lexer, TOK_OP, heap_str(1, cur));
          break;
        case '=':
          if (next == '=') {
            addtok(&lexer, TOK_OP, heap_str(2, cur, '='));
            readc(&lexer);
          } else
            addtok(&lexer, TOK_ASSIGN, heap_str(1, cur));
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
  char *val = strbuf_done(strbuf);
  vec_push(lexer->toks, tok_new(TOK_ID, val));
}

static void readnum(struct lexer *lexer) {
  struct strbuf *strbuf = strbuf_new();
  while (peekc(lexer) != -1 && isdigit((char)peekc(lexer)))
    strbuf_append(strbuf, (char)readc(lexer));
  char *val = strbuf_done(strbuf);
  vec_push(lexer->toks, tok_new(TOK_NUM, val));
}

static void readstr(struct lexer *lexer, char delin) {
  struct strbuf *strbuf = strbuf_new();
  readc(lexer);
  while (peekc(lexer) != -1 && (char)peekc(lexer) != delin)
    strbuf_append(strbuf, (char)readc(lexer));
  readc(lexer);
  char *val = strbuf_done(strbuf);
  vec_push(lexer->toks, tok_new(TOK_STRING, val));
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
  return lexer->code[lexer->pos++];
}

static void addtok(struct lexer *lexer, toktype_t type, char *val) {
  vec_push(lexer->toks, tok_new(type, val));
}

void free_toks(struct vec *toks) {
  for (int i = 0; i < toks->len; i++) tok_free(vec_get(toks, i));
  vec_free(toks);
}

void debug_toks(struct vec *toks) {
  for (int i = 0; i < toks->len; i++) {
    struct tok *tok = vec_get(toks, i);
    printf("Type: %d, Value: %s\n", tok->type, tok->val);
  }
}