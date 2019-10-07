#include <lexer/lexer.h>

struct lexer * lexer_init_from_src (char * code) {
    struct lexer * lexer;

    lexer         = (struct lexer *)calloc (1, sizeof (struct lexer));
    lexer->code   = code;
    lexer->pos    = 0;
    lexer->length = strlen (code);

    return lexer;
}

void free_lexer (struct lexer * lexer) {
    if (lexer->toks) {
        for (int i = 0; i < lexer->toks->length; i++) {
            free_token (vector_get (lexer->toks, i));
        }

        free_vector (lexer->toks);
    }
    free (lexer);
}

static void read_id  (struct lexer * lexer);
static void read_num (struct lexer * lexer);
static void read_str (struct lexer * lexer);

static void whitespace (struct lexer * lexer);

static void add_tok  (struct lexer * lexer, tok_type_t type, char * val);
static int peek_char (struct lexer * lexer);
static int next_char (struct lexer * lexer);
static int read_char (struct lexer * lexer);

struct vector * lexer_run (struct lexer * lexer) {
    char cur;
    char next;

    lexer->toks = vector_init ();

    while (peek_char (lexer) != -1) {
        whitespace (lexer);

        cur  = peek_char (lexer);
        next = next_char (lexer);

        if (isalpha (cur) || cur == '_') {
            read_id (lexer);
        } else if (isdigit (cur)) {
            read_num (lexer);
        } else {
            switch (cur) {
                case '"':
                    read_str (lexer);
                    break;
                case '=':
                    if (next == '=') {
                        add_tok (lexer, COMP_TOK, append_char (append_char (create_empty_str (), (char)read_char (lexer)), next));
                    } else {
                        add_tok (lexer, ASSIGN_TOK, append_char (create_empty_str (), cur));
                    }
                    break;
                case '}':
                    add_tok (lexer, CBRACE_TOK, NULL);
                    break;
                case ':':
                    add_tok (lexer, COLON_TOK, NULL);
                    break;
                case ',':
                    add_tok (lexer, COMMA_TOK, NULL);
                    break;
                case '<':
                case '>':
                    if (cur == next) {
                        add_tok (lexer, OP_TOK, append_char (append_char (create_empty_str (), (char)read_char (lexer)), next));
                    } else if (next == '=') {
                        add_tok (lexer, COMP_TOK, append_char (append_char (create_empty_str (), (char)read_char (lexer)), next));
                    } else {
                        add_tok (lexer, COMP_TOK, append_char (create_empty_str (), cur));
                    }
                    break;
                case '!':
                    if (next == '=') {
                        add_tok (lexer, COMP_TOK, append_char (append_char (create_empty_str (), (char)read_char (lexer)), next));
                    } else {
                        add_tok (lexer, OP_TOK, append_char (create_empty_str (), cur));
                    }
                    break;
                case ')':
                    add_tok (lexer, CPAREN_TOK, NULL);
                    break;
                case ']':
                    add_tok (lexer, CSQUARE_TOK, NULL);
                    break;
                case '.':
                    add_tok (lexer, DOT_TOK, NULL);
                    break;
                case '{':
                    add_tok (lexer, OBRACE_TOK, NULL);
                    break;
                case '+':
                case '-':
                case '*':
                case '/':
                case '%':
                    if (next == '=') {
                        add_tok (lexer, ASSIGN_TOK, append_char (append_char (create_empty_str (), (char)read_char (lexer)), next));
                    } else {
                        add_tok (lexer, OP_TOK, append_char (create_empty_str (), cur));
                    }
                    break;
                case '(':
                    add_tok (lexer, OPAREN_TOK, NULL);
                    break;
                case '[':
                    add_tok (lexer, OSQUARE_TOK, NULL);
                    break;
                case ';':
                    add_tok (lexer, SEMICOL_TOK, NULL);
                    break;
                default:
                    printf ("Unknown char \"%c\" %d\n", cur, (int)cur);
                    break;
            }
            read_char (lexer);
        }
    }

    return lexer->toks;
}

static void read_id (struct lexer * lexer) {
    char * str;

    str = create_empty_str ();
    while (isalnum ((char)peek_char (lexer)) || (char)peek_char (lexer) == '_') {
        str = append_char (str, (char)read_char (lexer));
    }

    add_tok (lexer, ID_TOK, str);
}

static void read_num (struct lexer * lexer) {
    char * str;

    str = create_empty_str ();
    while (isdigit ((char)peek_char (lexer)) || (char)peek_char (lexer) == '.') {
        str = append_char (str, (char)read_char (lexer));
    }

    add_tok (lexer, NUMBERC_TOK, str);
}

static void read_str (struct lexer * lexer) {
    char * str;


    read_char (lexer); // "

    str = create_empty_str ();
    while ((char)peek_char (lexer) != '"') {
        str = append_char (str, (char)read_char (lexer));
    }

    add_tok (lexer, STRINGC_TOK, str);
}

static void whitespace (struct lexer * lexer) {
    while (peek_char (lexer) != -1) {
        if (isspace ((char)peek_char (lexer)) == 0) {
            break;
        }

        read_char (lexer);
    }
}

static void add_tok (struct lexer * lexer, tok_type_t type, char * val) {
    vector_push (lexer->toks, token_init (type, val));
}

static int peek_char (struct lexer * lexer) {
    if (lexer->pos >= lexer->length) {
        return -1;
    }

    return lexer->code [lexer->pos];
}

static int next_char (struct lexer * lexer) {
    if (lexer->pos + 1 >= lexer->length) {
        return -1;
    }

    return lexer->code [lexer->pos + 1];
}

static int read_char (struct lexer * lexer) {
    if (lexer->pos >= lexer->length) {
        return -1;
    }

    return lexer->code [lexer->pos++];
}
