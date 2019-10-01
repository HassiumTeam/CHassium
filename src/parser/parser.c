#include <parser/parser.h>

struct parser * parser_init (struct vector * tokens) {
    struct parser * parser;

    parser         = (struct parser *)calloc (1, sizeof (struct parser));
    parser->pos    = 0;
    parser->tokens = tokens;

    return parser;
}

void free_parser (struct parser * parser) {
    free (parser);
}

static int eof                       (struct parser * parser);
static int match_tok                 (struct parser * parser, tok_type_t type);
static int match_tok_val             (struct parser * parser, tok_type_t type, char * val);
static int accept_tok                (struct parser * parser, tok_type_t type);
static int accept_tok_val            (struct parser * parser, tok_type_t type, char * val);
static struct token * expect_tok     (struct parser * parser, tok_type_t type);
static struct token * expect_tok_val (struct parser * parser, tok_type_t type, char * val);

struct ast_node * parser_run (struct parser * parser) {

}

static int eof (struct parser * parser) {
    return parser->pos >= parser->tokens->length;
}

static int match_tok (struct parser * parser, tok_type_t type) {
    struct token * cur;

    if (eof (parser)) {
        return -1;
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    return cur->type == type;
}

static int match_tok_val (struct parser * parser, tok_type_t type, char * val) {
    struct token * cur;

    if (eof (parser)) {
        return -1;
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    return cur->type == type && (strcmp (cur->val, val) == 0);
}

static int accept_tok (struct parser * parser, tok_type_t type) {
    if (match_tok (parser, type)) {
        parser->pos++;

        return 1;
    }

    return -1;
}

static int accept_tok_val (struct parser * parser, tok_type_t type, char * val) {
    if (match_tok_val (parser, type, val)) {
        parser->pos++;

        return 1;
    }

    return -1;
}

static struct token * expect_tok (struct parser * parser, tok_type_t type) {
    struct token * cur;

    if (match_tok (parser, type)) {
        return vector_get (parser->tokens, parser->pos++);
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    printf ("Parser error! Expected token with type %d, got %d with value \"%s\"!\n", type, cur->type, cur->val);
}

static struct token * expect_tok_val (struct parser * parser, tok_type_t type, char * val) {
    struct token * cur;

    if (match_tok_val (parser, type, val)) {
        return vector_get (parser->tokens, parser->pos++);
    }

    cur = (struct token *)vector_get (parser->tokens, parser->pos);

    printf ("Parser error! Expected token with type %d and value \"%s\", got %d with value \"%s\"!\n", type, val, cur->type, cur->val);
}
