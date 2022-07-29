#include <parser.h>

struct parser {
  struct vec *toks;
  int pos;
  int len;
};

static struct ast_node *parse_statement(struct parser *);
static struct ast_node *parse_block(struct parser *);
static struct ast_node *parse_class(struct parser *);
static struct ast_node *parse_for(struct parser *);
static struct ast_node *parse_foreach(struct parser *);
static struct ast_node *parse_func(struct parser *);
static struct ast_node *parse_if(struct parser *);
static struct ast_node *parse_import(struct parser *);
static struct ast_node *parse_raise(struct parser *);
static struct ast_node *parse_return(struct parser *);
static struct ast_node *parse_super(struct parser *);
static struct ast_node *parse_try(struct parser *);
static struct ast_node *parse_while(struct parser *);
static struct ast_node *parse_expr_stmt(struct parser *);
static struct ast_node *parse_expr(struct parser *);
static struct ast_node *parse_assign(struct parser *);
static struct ast_node *parse_or(struct parser *);
static struct ast_node *parse_and(struct parser *);
static struct ast_node *parse_equality(struct parser *);
static struct ast_node *parse_comp(struct parser *);
static struct ast_node *parse_add(struct parser *);
static struct ast_node *parse_mult(struct parser *);
static struct ast_node *parse_unary(struct parser *);
static struct ast_node *parse_access(struct parser *, struct ast_node *);
static struct ast_node *parse_array_decl(struct parser *);
static struct ast_node *parse_obj_decl(struct parser *);
static struct ast_node *parse_term(struct parser *);
static struct vec *parse_arg_list(struct parser *);

static struct tok *curtok(struct parser *);
static int matchtok(struct parser *, toktype_t);
static int matchtokv(struct parser *, toktype_t, char *);
static int accepttok(struct parser *, toktype_t);
static int accepttokv(struct parser *, toktype_t, char *);
static struct tok *expecttok(struct parser *, toktype_t);
static struct tok *expecttokv(struct parser *, toktype_t, char *);

struct ast_node *parser_parse(struct vec *toks) {
  struct parser parser;
  parser.toks = toks;
  parser.pos = 0;
  parser.len = parser.toks->len;

  struct vec *children = vec_new();
  while (parser.pos < parser.len) vec_push(children, parse_statement(&parser));
  return code_block_node_new(children);
}

static struct ast_node *parse_statement(struct parser *parser) {
  struct ast_node *ret;
  if (matchtok(parser, TOK_OBRACE))
    ret = parse_block(parser);
  else if (accepttokv(parser, TOK_ID, "break"))
    return ast_node_new(BREAK_NODE, NULL);
  else if (matchtokv(parser, TOK_ID, "class"))
    ret = parse_class(parser);
  else if (accepttokv(parser, TOK_ID, "continue"))
    return ast_node_new(CONTINUE_NODE, NULL);
  else if (matchtokv(parser, TOK_ID, "for"))
    ret = parse_for(parser);
  else if (matchtokv(parser, TOK_ID, "foreach"))
    ret = parse_foreach(parser);
  else if (matchtokv(parser, TOK_ID, "func"))
    ret = parse_func(parser);
  else if (matchtok(parser, TOK_SEMICOLON))
    return ast_node_new(EMPTY_STMT, NULL);
  else if (matchtokv(parser, TOK_ID, "if"))
    ret = parse_if(parser);
  else if (matchtokv(parser, TOK_ID, "import"))
    ret = parse_import(parser);
  else if (matchtokv(parser, TOK_ID, "raise"))
    ret = parse_raise(parser);
  else if (matchtokv(parser, TOK_ID, "return"))
    ret = parse_return(parser);
  else if (matchtokv(parser, TOK_ID, "super"))
    ret = parse_super(parser);
  else if (matchtokv(parser, TOK_ID, "try"))
    ret = parse_try(parser);
  else if (matchtokv(parser, TOK_ID, "while"))
    ret = parse_while(parser);
  else
    ret = parse_expr_stmt(parser);
  accepttok(parser, TOK_SEMICOLON);
  return ret;
}

static struct ast_node *parse_block(struct parser *parser) {
  struct vec *children = vec_new();
  expecttok(parser, TOK_OBRACE);
  while (!accepttok(parser, TOK_CBRACE))
    vec_push(children, parse_statement(parser));
  return code_block_node_new(children);
}

static struct ast_node *parse_class(struct parser *parser) {
  expecttokv(parser, TOK_ID, "class");
  char *id = expecttok(parser, TOK_ID)->val;
  struct ast_node *extends;
  if (accepttokv(parser, TOK_ID, "extends")) extends = parse_expr(parser);
  struct ast_node *body = parse_statement(parser);
}

static struct ast_node *parse_for(struct parser *parser) {
  expecttokv(parser, TOK_ID, "for");
  bool using_parens = accepttok(parser, TOK_OPAREN);
  struct ast_node *initial = parse_expr_stmt(parser);
  accepttok(parser, TOK_SEMICOLON);
  struct ast_node *condition = parse_expr(parser);
  accepttok(parser, TOK_SEMICOLON);
  struct ast_node *repeat = parse_expr_stmt(parser);
  if (using_parens) expecttok(parser, TOK_CPAREN);
  struct ast_node *body = parse_statement(parser);
  return for_node_new(initial, condition, repeat, body);
}

static struct ast_node *parse_foreach(struct parser *parser) {
  expecttokv(parser, TOK_ID, "foreach");
  bool using_parens = accepttok(parser, TOK_OPAREN);
  char *id = clone_str(expecttok(parser, TOK_ID)->val);
  expecttokv(parser, TOK_ID, "in");
  struct ast_node *target = parse_expr(parser);
  struct ast_node *body = parse_statement(parser);
  return foreach_node_new(id, target, body);
}

static struct ast_node *parse_func(struct parser *parser) {
  expecttokv(parser, TOK_ID, "func");
  char *name = clone_str(expecttok(parser, TOK_ID)->val);
  expecttok(parser, TOK_OPAREN);
  struct vec *args = vec_new();
  while (!accepttok(parser, TOK_CPAREN)) {
    vec_push(args, clone_str(expecttok(parser, TOK_ID)->val));
    accepttok(parser, TOK_COMMA);
  }
  struct ast_node *ret_type = NULL;
  if (accepttok(parser, TOK_COLON)) ret_type = parse_expr(parser);
  struct ast_node *body = parse_statement(parser);
  return func_decl_node_new(name, args, ret_type, body);
}

static struct ast_node *parse_if(struct parser *parser) {
  expecttokv(parser, TOK_ID, "if");
  bool using_parens = accepttok(parser, TOK_OPAREN);
  struct ast_node *predicate = parse_expr(parser);
  if (using_parens) expecttok(parser, TOK_CPAREN);
  struct ast_node *body = parse_statement(parser);
  struct ast_node *else_body = NULL;
  if (accepttokv(parser, TOK_ID, "else")) else_body = parse_statement(parser);
  return if_node_new(predicate, body, else_body);
}

static struct ast_node *parse_import(struct parser *parser) {
  expecttokv(parser, TOK_ID, "import");
  struct ast_node *target = parse_expr(parser);
  return import_node_new(target);
}

static struct ast_node *parse_raise(struct parser *parser) {
  expecttokv(parser, TOK_ID, "raise");
  struct ast_node *value = parse_expr(parser);
  return raise_node_new(value);
}

static struct ast_node *parse_return(struct parser *parser) {
  expecttokv(parser, TOK_ID, "return");
  struct ast_node *value = parse_expr(parser);
  return return_node_new(value);
}

static struct ast_node *parse_super(struct parser *parser) {
  expecttokv(parser, TOK_ID, "super");
  return super_node_new(parse_arg_list(parser));
}

static struct ast_node *parse_try(struct parser *parser) {
  expecttokv(parser, TOK_ID, "try");
  struct ast_node *try = parse_statement(parser);
  expecttokv(parser, TOK_ID, "catch");
  bool using_parens = accepttok(parser, TOK_OPAREN);
  char *id = clone_str(expecttok(parser, TOK_ID)->val);
  if (using_parens) expecttok(parser, TOK_CPAREN);
  struct ast_node *catch = parse_statement(parser);
  return try_catch_node_new(try, catch, id);
}

static struct ast_node *parse_while(struct parser *parser) {
  expecttokv(parser, TOK_ID, "while");
  struct ast_node *condition = parse_expr(parser);
  struct ast_node *body = parse_statement(parser);
  return while_node_new(condition, body);
}

static struct ast_node *parse_expr_stmt(struct parser *parser) {
  struct ast_node *expr = parse_expr(parser);
  return expr_stmt_node_new(expr);
}

static struct ast_node *parse_expr(struct parser *parser) {
  return parse_assign(parser);
}

static struct ast_node *parse_assign(struct parser *parser) {
  struct ast_node *left = parse_or(parser);
  if (accepttokv(parser, TOK_ASSIGN, "=")) {
    return bin_op_node_new(BIN_OP_ASSIGN, left, parse_assign(parser));
  } else if (matchtok(parser, TOK_ASSIGN)) {
    char *op = expecttok(parser, TOK_ASSIGN)->val;
    bin_op_type_t op_type;
    if (strcmp(op, "+=") == 0)
      op_type = BIN_OP_ADD;
    else if (strcmp(op, "-=") == 0)
      op_type = BIN_OP_SUB;
    else if (strcmp(op, "*=") == 0)
      op_type = BIN_OP_MUL;
    else if (strcmp(op, "/=") == 0)
      op_type = BIN_OP_DIV;
    else if (strcmp(op, "%=") == 0)
      op_type = BIN_OP_MOD;
    else if (strcmp(op, "&=") == 0)
      op_type = BIN_OP_AND;
    else if (strcmp(op, "|=") == 0)
      op_type = BIN_OP_OR;

    return bin_op_node_new(
        BIN_OP_ASSIGN, left,
        bin_op_node_new(op_type, left, parse_assign(parser)));
  }
  return left;
}

static struct ast_node *parse_or(struct parser *parser) {
  struct ast_node *left = parse_and(parser);
  if (accepttokv(parser, TOK_OP, "||"))
    return bin_op_node_new(BIN_OP_OR, left, parse_or(parser));
  return left;
}

static struct ast_node *parse_and(struct parser *parser) {
  struct ast_node *left = parse_equality(parser);
  if (accepttokv(parser, TOK_OP, "&&"))
    return bin_op_node_new(BIN_OP_AND, left, parse_or(parser));
  return left;
}

static struct ast_node *parse_equality(struct parser *parser) {
  struct ast_node *left = parse_comp(parser);
  if (accepttokv(parser, TOK_OP, "=="))
    return bin_op_node_new(BIN_OP_EQ, left, parse_equality(parser));
  else if (accepttokv(parser, TOK_OP, "!="))
    return unary_op_node_new(
        UNARY_OP_NOT, bin_op_node_new(BIN_OP_EQ, left, parse_equality(parser)));
  return left;
}

static struct ast_node *parse_comp(struct parser *parser) {
  struct ast_node *left = parse_add(parser);
  if (accepttokv(parser, TOK_OP, ">"))
    return bin_op_node_new(BIN_OP_GREATER, left, parse_comp(parser));
  else if (accepttokv(parser, TOK_OP, ">="))
    return bin_op_node_new(BIN_OP_GREATER_OR_EQ, left, parse_comp(parser));
  else if (accepttokv(parser, TOK_OP, "<"))
    return bin_op_node_new(BIN_OP_LESSER, left, parse_comp(parser));
  else if (accepttokv(parser, TOK_OP, "<="))
    return bin_op_node_new(BIN_OP_LESSER_OR_EQ, left, parse_comp(parser));
  return left;
}

static struct ast_node *parse_add(struct parser *parser) {
  struct ast_node *left = parse_mult(parser);
  if (accepttokv(parser, TOK_OP, "+"))
    return bin_op_node_new(BIN_OP_ADD, left, parse_add(parser));
  else if (accepttokv(parser, TOK_OP, "-"))
    return bin_op_node_new(BIN_OP_SUB, left, parse_add(parser));
  return left;
}

static struct ast_node *parse_mult(struct parser *parser) {
  struct ast_node *left = parse_unary(parser);
  if (accepttokv(parser, TOK_OP, "*"))
    return bin_op_node_new(BIN_OP_MUL, left, parse_mult(parser));
  else if (accepttokv(parser, TOK_OP, "/"))
    return bin_op_node_new(BIN_OP_DIV, left, parse_mult(parser));
  else if (accepttokv(parser, TOK_OP, "%"))
    return bin_op_node_new(BIN_OP_MOD, left, parse_mult(parser));
  return left;
}

static struct ast_node *parse_unary(struct parser *parser) {
  if (accepttokv(parser, TOK_OP, "!"))
    return unary_op_node_new(UNARY_OP_NOT, parse_unary(parser));
  return parse_access(parser, NULL);
}
static struct ast_node *parse_access(struct parser *parser,
                                     struct ast_node *left) {
  if (left == NULL) left = parse_array_decl(parser);
  if (accepttok(parser, TOK_DOT))
    return parse_access(
        parser,
        attrib_node_new(left, clone_str(expecttok(parser, TOK_ID)->val)));

  else if (accepttok(parser, TOK_OPAREN))
    return parse_access(parser, invoke_node_new(left, parse_arg_list(parser)));
  else if (accepttok(parser, TOK_OSQUARE)) {
    struct ast_node *key = parse_expr(parser);
    expecttok(parser, TOK_CSQUARE);
    return parse_access(parser, subscript_node_new(left, key));
  }
  return left;
}

static struct ast_node *parse_array_decl(struct parser *parser) {
  if (accepttok(parser, TOK_OSQUARE)) {
    struct vec *values = vec_new();
    while (!accepttok(parser, TOK_CSQUARE)) {
      vec_push(values, parse_expr(parser));
      accepttok(parser, TOK_COMMA);
    }
    return array_decl_node_new(values);
  } else {
    return parse_obj_decl(parser);
  }
}

static struct ast_node *parse_obj_decl(struct parser *parser) {
  if (accepttok(parser, TOK_OBRACE)) {
    struct vec *keys = vec_new();
    struct vec *values = vec_new();
    while (!accepttok(parser, TOK_CBRACE)) {
      char *key;
      bool id = false;
      if (matchtok(parser, TOK_ID)) {
        key = clone_str(expecttok(parser, TOK_ID)->val);
        id = true;
      } else if (matchtok(parser, TOK_STRING)) {
        key = clone_str(expecttok(parser, TOK_STRING)->val);
      } else {
        // throw an error
        expecttok(parser, TOK_ID);
      }
      vec_push(keys, key);

      if (id && !accepttok(parser, TOK_COLON)) {
        vec_push(values, id_node_new(clone_str(key)));
      } else {
        vec_push(values, parse_expr(parser));
      }
      accepttok(parser, TOK_COMMA);
    }

    return obj_decl_node_new(keys, values);
  } else {
    return parse_term(parser);
  }
}

static struct ast_node *parse_term(struct parser *parser) {
  if (accepttok(parser, TOK_OPAREN)) {
    struct ast_node *expr = parse_expr(parser);
    expecttok(parser, TOK_CPAREN);
    return expr;
  } else if (matchtok(parser, TOK_ID))
    return id_node_new(clone_str(expecttok(parser, TOK_ID)->val));
  else if (matchtok(parser, TOK_NUM))
    return num_node_new(atof(expecttok(parser, TOK_NUM)->val));
  else if (matchtok(parser, TOK_STRING))
    return string_node_new(clone_str(expecttok(parser, TOK_STRING)->val));
  struct tok *tok = curtok(parser);
  printf("Unexpected %d '%s'\n", tok->type, tok->val);
}

static struct vec *parse_arg_list(struct parser *parser) {
  struct vec *args = vec_new();
  while (!accepttok(parser, TOK_CPAREN)) {
    vec_push(args, parse_expr(parser));
    accepttok(parser, TOK_COMMA);
  }
  return args;
}

static struct tok *curtok(struct parser *parser) {
  if (parser->pos >= parser->len) return NULL;
  return vec_get(parser->toks, parser->pos);
}

static int matchtok(struct parser *parser, toktype_t type) {
  struct tok *tok = curtok(parser);
  if (tok == NULL) return false;
  return tok->type == type;
}

static int matchtokv(struct parser *parser, toktype_t type, char *val) {
  struct tok *tok = curtok(parser);
  if (tok == NULL) return false;
  return tok->type == type && strcmp(tok->val, val) == 0;
}

static int accepttok(struct parser *parser, toktype_t type) {
  if (matchtok(parser, type)) {
    parser->pos++;
    return true;
  }
  return false;
}

static int accepttokv(struct parser *parser, toktype_t type, char *val) {
  if (matchtokv(parser, type, val)) {
    parser->pos++;
    return true;
  }
  return false;
}

static struct tok *expecttok(struct parser *parser, toktype_t type) {
  if (matchtok(parser, type)) {
    struct tok *ret = curtok(parser);
    parser->pos++;
    return ret;
  }
  printf("Expected token %d\n", type);
  exit(-1);
}

static struct tok *expecttokv(struct parser *parser, toktype_t type,
                              char *val) {
  if (matchtokv(parser, type, val)) {
    struct tok *ret = curtok(parser);
    parser->pos++;
    return ret;
  }
  printf("Expected token %d %s\n", type, val);
  exit(-1);
}