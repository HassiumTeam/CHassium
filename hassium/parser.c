#include <parser.h>

struct parser {
  struct vec *toks;
  int pos;
  int len;
  struct vm *vm;
};

static struct ast_node *parse_statement(struct parser *);
static struct ast_node *parse_block(struct parser *);
static struct ast_node *parse_class(struct parser *);
static struct ast_node *parse_delete(struct parser *);
static struct ast_node *parse_do(struct parser *);
static struct ast_node *parse_for(struct parser *);
static struct ast_node *parse_foreach(struct parser *);
static struct ast_node *parse_func(struct parser *);
static struct ast_node *parse_if(struct parser *);
static struct ast_node *parse_import(struct parser *);
static struct ast_node *parse_raise(struct parser *);
static struct ast_node *parse_return(struct parser *);
static struct ast_node *parse_super(struct parser *);
static struct ast_node *parse_switch(struct parser *);
static struct ast_node *parse_try(struct parser *);
static struct ast_node *parse_until(struct parser *);
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

#define CURRENT_SOURCEPOS() \
  (curtok(parser) == NULL ? NULL : curtok(parser)->sourcepos)
#define eof(p) (p->pos >= p->toks->len)
#define lasttok(p) ((struct tok *)vec_get(p->toks, p->toks->len - 1))
#define peektok(p, o)                                                       \
  (p->pos + o < p->toks->len ? ((struct tok *)vec_get(p->toks, p->pos + o)) \
                             : NULL)

static struct tok *curtok(struct parser *);
static int matchtok(struct parser *, toktype_t);
static int matchtokv(struct parser *, toktype_t, char *);
static int accepttok(struct parser *, toktype_t);
static int accepttokv(struct parser *, toktype_t, char *);
static struct tok *expecttok(struct parser *, toktype_t);
static struct tok *expecttokv(struct parser *, toktype_t, char *);

struct ast_node *parser_parse(struct vec *toks, struct vm *vm) {
  struct parser parser;
  parser.toks = toks;
  parser.pos = 0;
  parser.len = parser.toks->len;
  parser.vm = vm;

  struct sourcepos *sourcepos = ((struct tok *)vec_get(toks, 0))->sourcepos;
  struct vec *children = vec_new();
  while (parser.pos < parser.len) {
    vec_push(children, parse_statement(&parser));
  }
  return code_block_node_new(children, sourcepos);
}

static struct ast_node *parse_statement(struct parser *parser) {
  if (eof(parser)) {
    vm_raise(parser->vm, obj_compile_error_new("Unexpected end-of-file",
                                               (lasttok(parser))->sourcepos));
    return NULL;
  }

  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();
  struct ast_node *ret;

  // This big chain is to support { a, b } = o; syntax
  if (peektok(parser, 1) && peektok(parser, 2) &&
      matchtok(parser, TOK_OBRACE) && peektok(parser, 1)->type == TOK_ID &&
      (peektok(parser, 2)->type == TOK_COMMA ||
       peektok(parser, 2)->type == TOK_COLON ||
       peektok(parser, 2)->type == TOK_CBRACE)) {
    ret = parse_expr_stmt(parser);
  } else if (matchtok(parser, TOK_OBRACE))
    ret = parse_block(parser);
  else if (accepttokv(parser, TOK_ID, "break"))
    ret = ast_node_new(BREAK_NODE, NULL, sourcepos);
  else if (matchtokv(parser, TOK_ID, "class"))
    ret = parse_class(parser);
  else if (accepttokv(parser, TOK_ID, "continue"))
    ret = ast_node_new(CONTINUE_NODE, NULL, sourcepos);
  else if (matchtokv(parser, TOK_ID, "delete"))
    ret = parse_delete(parser);
  else if (matchtokv(parser, TOK_ID, "do"))
    ret = parse_do(parser);
  else if (matchtokv(parser, TOK_ID, "for"))
    ret = parse_for(parser);
  else if (matchtokv(parser, TOK_ID, "foreach"))
    ret = parse_foreach(parser);
  else if (matchtokv(parser, TOK_ID, "func"))
    ret = parse_func(parser);
  else if (matchtok(parser, TOK_SEMICOLON))
    return ast_node_new(EMPTY_STMT, NULL, sourcepos);
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
  else if (matchtokv(parser, TOK_ID, "switch"))
    ret = parse_switch(parser);
  else if (matchtokv(parser, TOK_ID, "try"))
    ret = parse_try(parser);
  else if (matchtokv(parser, TOK_ID, "until"))
    ret = parse_until(parser);
  else if (matchtokv(parser, TOK_ID, "while"))
    ret = parse_while(parser);
  else
    ret = parse_expr_stmt(parser);

  accepttok(parser, TOK_SEMICOLON);

  return ret;
}

static struct ast_node *parse_block(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  struct vec *children = vec_new();
  expecttok(parser, TOK_OBRACE);
  while (!accepttok(parser, TOK_CBRACE))
    vec_push(children, parse_statement(parser));
  return code_block_node_new(children, sourcepos);
}

static struct ast_node *parse_class(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "class");
  char *id = clone_str(expecttok(parser, TOK_ID)->val);

  struct ast_node *extends = NULL;
  if (accepttokv(parser, TOK_ID, "extends")) {
    extends = parse_expr(parser);
  }

  struct vec *impls = vec_new();
  if (accepttok(parser, TOK_COLON)) {
    while (!matchtok(parser, TOK_OBRACE)) {
      vec_push(impls, parse_expr(parser));
    }
  }

  struct ast_node *body = parse_statement(parser);

  return class_decl_node_new(id, extends, body, impls, sourcepos);
}

static struct ast_node *parse_delete(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "delete");
  struct ast_node *target = parse_expr(parser);
  return delete_node_new(target, sourcepos);
}

static struct ast_node *parse_do(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "do");
  struct ast_node *body = parse_statement(parser);
  expecttokv(parser, TOK_ID, "while");
  struct ast_node *condition = parse_expr(parser);
  return do_while_node_new(body, condition, sourcepos);
}

static struct ast_node *parse_for(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "for");
  bool using_parens = accepttok(parser, TOK_OPAREN);
  struct ast_node *initial = parse_expr_stmt(parser);
  accepttok(parser, TOK_SEMICOLON);
  struct ast_node *condition = parse_expr(parser);
  accepttok(parser, TOK_SEMICOLON);
  struct ast_node *repeat = parse_expr_stmt(parser);
  if (using_parens) expecttok(parser, TOK_CPAREN);
  struct ast_node *body = parse_statement(parser);
  return for_node_new(initial, condition, repeat, body, sourcepos);
}

static struct ast_node *parse_foreach(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "foreach");
  bool using_parens = accepttok(parser, TOK_OPAREN);
  char *id = clone_str(expecttok(parser, TOK_ID)->val);
  expecttokv(parser, TOK_ID, "in");
  struct ast_node *target = parse_expr(parser);
  if (using_parens) expecttok(parser, TOK_CPAREN);
  struct ast_node *body = parse_statement(parser);
  return foreach_node_new(id, target, body, sourcepos);
}

static struct ast_node *parse_func(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttok(parser, TOK_ID);

  char *name = NULL;
  if (matchtok(parser, TOK_ID)) {
    name = clone_str(expecttok(parser, TOK_ID)->val);
  }

  expecttok(parser, TOK_OPAREN);
  struct vec *args = vec_new();
  while (!accepttok(parser, TOK_CPAREN)) {
    struct sourcepos *param_sourcepos = CURRENT_SOURCEPOS();
    char *id = clone_str(expecttok(parser, TOK_ID)->val);
    struct ast_node *type = NULL;
    if (accepttok(parser, TOK_COLON) && !accepttokv(parser, TOK_ID, "any")) {
      type = parse_expr(parser);
    }
    vec_push(args, id_node_new(id, type, param_sourcepos));
    accepttok(parser, TOK_COMMA);
  }

  struct ast_node *ret_type = NULL;
  if (accepttok(parser, TOK_COLON) && !accepttokv(parser, TOK_ID, "any")) {
    ret_type = parse_expr(parser);
  }

  struct ast_node *body;
  if (matchtok(parser, TOK_OBRACE)) {
    body = parse_statement(parser);
  } else {
    body = return_node_new(parse_expr(parser), CURRENT_SOURCEPOS());
  }

  return func_decl_node_new(name, args, ret_type, body, sourcepos);
}

static struct ast_node *parse_if(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "if");

  bool using_parens = accepttok(parser, TOK_OPAREN);
  struct ast_node *predicate = parse_expr(parser);
  if (using_parens) expecttok(parser, TOK_CPAREN);

  struct ast_node *body = parse_statement(parser);
  struct ast_node *else_body = NULL;

  if (accepttokv(parser, TOK_ID, "else")) {
    else_body = parse_statement(parser);
  }

  return if_node_new(predicate, body, else_body, sourcepos);
}

static struct ast_node *parse_import(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "import");
  struct vec *imports = vec_new();
  if (!accepttokv(parser, TOK_OP, "*")) {
    vec_push(imports, clone_str(expecttok(parser, TOK_ID)->val));
    while (accepttok(parser, TOK_COMMA)) {
      vec_push(imports, clone_str(expecttok(parser, TOK_ID)->val));
    }
    accepttok(parser, TOK_COMMA);
  }

  expecttokv(parser, TOK_ID, "from");
  struct vec *from = vec_new();
  vec_push(from, clone_str(expecttok(parser, TOK_ID)->val));
  while (accepttok(parser, TOK_DOT)) {
    vec_push(from, clone_str(expecttok(parser, TOK_ID)->val));
  }

  return import_node_new(imports, from, sourcepos);
}

static struct ast_node *parse_raise(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "raise");
  struct ast_node *value = parse_expr(parser);

  return raise_node_new(value, sourcepos);
}

static struct ast_node *parse_return(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "return");
  struct ast_node *value = parse_expr(parser);
  return return_node_new(value, sourcepos);
}

static struct ast_node *parse_super(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "super");
  expecttok(parser, TOK_OPAREN);
  return expr_stmt_node_new(
      invoke_node_new(
          attrib_node_new(id_node_new(clone_str("self"), NULL, sourcepos),
                          clone_str("__super__"), sourcepos),
          parse_arg_list(parser), sourcepos),
      sourcepos);
}

static struct ast_node *parse_switch(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();
  struct vec *cases = vec_new();
  struct vec *case_bodies = vec_new();
  struct ast_node *default_case = NULL;

  expecttokv(parser, TOK_ID, "switch");
  struct ast_node *target = parse_expr(parser);

  expecttok(parser, TOK_OBRACE);
  while (!accepttok(parser, TOK_CBRACE)) {
    if (matchtokv(parser, TOK_ID, "default")) {
      if (default_case != NULL) {
        vm_raise(parser->vm,
                 obj_compile_error_new("Cannot have two default cases!",
                                       CURRENT_SOURCEPOS()));
      }
      expecttokv(parser, TOK_ID, "default");

      expecttok(parser, TOK_COLON);
      struct sourcepos *default_body_sourcepos = CURRENT_SOURCEPOS();
      struct vec *default_body = vec_new();

      while (!matchtokv(parser, TOK_ID, "break") &&
             !matchtokv(parser, TOK_ID, "return")) {
        vec_push(default_body, parse_statement(parser));
      }
      vec_push(default_body, parse_statement(parser));

      default_case = code_block_node_new(default_body, default_body_sourcepos);

    } else {
      expecttokv(parser, TOK_ID, "case");
      struct sourcepos *case_sourcepos = CURRENT_SOURCEPOS();
      struct ast_node *case_ = NULL;

      if (accepttokv(parser, TOK_OP, "!") && matchtok(parser, TOK_COLON)) {
        case_ = unary_op_node_new(UNARY_OP_NOT, NULL, true, case_sourcepos);
      } else if (matchtok(parser, TOK_OP)) {
        bin_op_type_t bin_op_type;
        if (accepttokv(parser, TOK_OP, "&&")) {
          bin_op_type = BIN_OP_AND;
        } else if (accepttokv(parser, TOK_OP, "==")) {
          bin_op_type = BIN_OP_EQ;
        } else if (accepttokv(parser, TOK_OP, ">")) {
          bin_op_type = BIN_OP_GREATER;
        } else if (accepttokv(parser, TOK_OP, ">=")) {
          bin_op_type = BIN_OP_GREATER_OR_EQ;
        } else if (accepttokv(parser, TOK_OP, "<")) {
          bin_op_type = BIN_OP_LESSER;
        } else if (accepttokv(parser, TOK_OP, "<=")) {
          bin_op_type = BIN_OP_LESSER_OR_EQ;
        } else if (accepttokv(parser, TOK_OP, "||")) {
          bin_op_type = BIN_OP_OR;
        } else {
          struct strbuf *strbuf = strbuf_new();
          strbuf_append_str(strbuf, "Invalid binary operator '");
          strbuf_append_str(strbuf, curtok(parser)->val);
          strbuf_append_str(strbuf, "'");
          char *message = strbuf_done(strbuf);
          vm_raise(parser->vm,
                   obj_compile_error_new(message, curtok(parser)->sourcepos));
          free(message);
        }

        case_ = bin_op_node_new(bin_op_type, NULL, parse_expr(parser), true,
                                case_sourcepos);
      } else {
        case_ = parse_expr(parser);
      }
      vec_push(cases, case_);
      expecttok(parser, TOK_COLON);

      if (matchtokv(parser, TOK_ID, "case")) {
        vec_push(case_bodies, NULL);
      } else {
        struct sourcepos *case_body_sourcepos = CURRENT_SOURCEPOS();
        struct vec *case_body = vec_new();

        while (!matchtokv(parser, TOK_ID, "break") &&
               !matchtokv(parser, TOK_ID, "return")) {
          vec_push(case_body, parse_statement(parser));
        }
        vec_push(case_body, parse_statement(parser));

        vec_push(case_bodies,
                 code_block_node_new(case_body, case_body_sourcepos));
      }
    }
  }

  return switch_node_new(target, cases, case_bodies, default_case, sourcepos);
}

static struct ast_node *parse_try(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "try");
  struct ast_node *try = parse_statement(parser);
  expecttokv(parser, TOK_ID, "catch");

  char *id = NULL;
  if (accepttok(parser, TOK_OPAREN)) {
    id = clone_str(expecttok(parser, TOK_ID)->val);
    expecttok(parser, TOK_CPAREN);
  }

  struct ast_node *catch = parse_statement(parser);

  return try_catch_node_new(try, catch, id, sourcepos);
}

static struct ast_node *parse_until(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "until");
  struct ast_node *condition = parse_expr(parser);
  struct ast_node *body = parse_statement(parser);
  struct ast_node *else_body = NULL;

  if (accepttokv(parser, TOK_ID, "else")) {
    else_body = parse_statement(parser);
  }

  return while_node_new(
      unary_op_node_new(UNARY_OP_NOT, condition, false, condition->sourcepos),
      body, else_body, sourcepos);
}

static struct ast_node *parse_while(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  expecttokv(parser, TOK_ID, "while");
  struct ast_node *condition = parse_expr(parser);
  struct ast_node *body = parse_statement(parser);
  struct ast_node *else_body = NULL;

  if (accepttokv(parser, TOK_ID, "else")) {
    else_body = parse_statement(parser);
  }

  return while_node_new(condition, body, else_body, sourcepos);
}

static struct ast_node *parse_expr_stmt(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();
  struct ast_node *expr = parse_expr(parser);

  return expr_stmt_node_new(expr, sourcepos);
}

static struct ast_node *parse_expr(struct parser *parser) {
  if (eof(parser)) {
    vm_raise(parser->vm, obj_compile_error_new("Unexpected end-of-file",
                                               (lasttok(parser))->sourcepos));
    return NULL;
  }

  return parse_assign(parser);
}

static struct ast_node *parse_assign(struct parser *parser) {
  struct ast_node *left = parse_or(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_ASSIGN, "=")) {
    return bin_op_node_new(BIN_OP_ASSIGN, left, parse_assign(parser), false,
                           sourcepos);
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
        BIN_OP_ASSIGN_SHORT, left,
        bin_op_node_new(op_type, left, parse_assign(parser), false, sourcepos),
        false, sourcepos);
  }
  return left;
}

static struct ast_node *parse_or(struct parser *parser) {
  struct ast_node *left = parse_and(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_OP, "||"))
    return bin_op_node_new(BIN_OP_OR, left, parse_or(parser), false, sourcepos);
  return left;
}

static struct ast_node *parse_and(struct parser *parser) {
  struct ast_node *left = parse_equality(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_OP, "&&"))
    return bin_op_node_new(BIN_OP_AND, left, parse_or(parser), false,
                           sourcepos);
  return left;
}

static struct ast_node *parse_equality(struct parser *parser) {
  struct ast_node *left = parse_comp(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_OP, "=="))
    return bin_op_node_new(BIN_OP_EQ, left, parse_equality(parser), false,
                           sourcepos);
  else if (accepttokv(parser, TOK_OP, "!="))
    return unary_op_node_new(
        UNARY_OP_NOT,
        bin_op_node_new(BIN_OP_EQ, left, parse_equality(parser), false,
                        sourcepos),
        false, sourcepos);
  return left;
}

static struct ast_node *parse_comp(struct parser *parser) {
  struct ast_node *left = parse_add(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_OP, ">"))
    return bin_op_node_new(BIN_OP_GREATER, left, parse_comp(parser), false,
                           sourcepos);
  else if (accepttokv(parser, TOK_OP, ">="))
    return bin_op_node_new(BIN_OP_GREATER_OR_EQ, left, parse_comp(parser),
                           false, sourcepos);
  else if (accepttokv(parser, TOK_OP, "<"))
    return bin_op_node_new(BIN_OP_LESSER, left, parse_comp(parser), false,
                           sourcepos);
  else if (accepttokv(parser, TOK_OP, "<="))
    return bin_op_node_new(BIN_OP_LESSER_OR_EQ, left, parse_comp(parser), false,
                           sourcepos);
  return left;
}

static struct ast_node *parse_add(struct parser *parser) {
  struct ast_node *left = parse_mult(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_OP, "+"))
    return bin_op_node_new(BIN_OP_ADD, left, parse_add(parser), false,
                           sourcepos);
  else if (accepttokv(parser, TOK_OP, "-"))
    return bin_op_node_new(BIN_OP_SUB, left, parse_add(parser), false,
                           sourcepos);
  return left;
}

static struct ast_node *parse_mult(struct parser *parser) {
  struct ast_node *left = parse_unary(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_OP, "*"))
    return bin_op_node_new(BIN_OP_MUL, left, parse_mult(parser), false,
                           sourcepos);
  else if (accepttokv(parser, TOK_OP, "/"))
    return bin_op_node_new(BIN_OP_DIV, left, parse_mult(parser), false,
                           sourcepos);
  else if (accepttokv(parser, TOK_OP, "%"))
    return bin_op_node_new(BIN_OP_MOD, left, parse_mult(parser), false,
                           sourcepos);
  return left;
}

static struct ast_node *parse_unary(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttokv(parser, TOK_OP, "!")) {
    return unary_op_node_new(UNARY_OP_NOT, parse_unary(parser), false,
                             sourcepos);
  } else if (accepttokv(parser, TOK_OP, "--")) {
    struct ast_node *target = parse_unary(parser);
    return bin_op_node_new(
        BIN_OP_ASSIGN_SHORT, target,
        bin_op_node_new(BIN_OP_SUB, target,
                        num_node_new(false, 1, 0, sourcepos), false, sourcepos),
        false, sourcepos);
  } else if (accepttokv(parser, TOK_OP, "++")) {
    struct ast_node *target = parse_unary(parser);
    return bin_op_node_new(
        BIN_OP_ASSIGN_SHORT, target,
        bin_op_node_new(BIN_OP_ADD, target,
                        num_node_new(false, 1, 0, sourcepos), false, sourcepos),
        false, sourcepos);
  }
  struct ast_node *target = parse_access(parser, NULL);
  if (accepttokv(parser, TOK_OP, "--")) {
    return unary_op_node_new(
        UNARY_OP_POST_DEC,
        bin_op_node_new(BIN_OP_ASSIGN_SHORT, target,
                        bin_op_node_new(BIN_OP_SUB, target,
                                        num_node_new(false, 1, 0, sourcepos),
                                        false, sourcepos),
                        false, sourcepos),
        false, sourcepos);
  } else if (accepttokv(parser, TOK_OP, "++")) {
    return unary_op_node_new(
        UNARY_OP_POST_DEC,
        bin_op_node_new(BIN_OP_ASSIGN_SHORT, target,
                        bin_op_node_new(BIN_OP_ADD, target,
                                        num_node_new(false, 1, 0, sourcepos),
                                        false, sourcepos),
                        false, sourcepos),
        false, sourcepos);
  }

  return target;
}
static struct ast_node *parse_access(struct parser *parser,
                                     struct ast_node *left) {
  if (left == NULL) left = parse_array_decl(parser);
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttok(parser, TOK_DOT))
    return parse_access(
        parser, attrib_node_new(left, clone_str(expecttok(parser, TOK_ID)->val),
                                sourcepos));
  else if (accepttok(parser, TOK_OPAREN)) {
    return parse_access(
        parser, invoke_node_new(left, parse_arg_list(parser), sourcepos));
  } else if (accepttok(parser, TOK_OSQUARE)) {
    if (accepttok(parser, TOK_COLON)) {
      if (accepttok(parser, TOK_CSQUARE)) {
        return parse_access(parser,
                            slice_node_new(left, NULL, NULL, sourcepos));
      }
      struct ast_node *end = parse_expr(parser);
      expecttok(parser, TOK_CSQUARE);
      return parse_access(parser, slice_node_new(left, NULL, end, sourcepos));
    } else {
      struct ast_node *key_or_start = parse_expr(parser);
      if (accepttok(parser, TOK_COLON)) {
        if (accepttok(parser, TOK_CSQUARE)) {
          return parse_access(
              parser, slice_node_new(left, key_or_start, NULL, sourcepos));
        } else {
          struct ast_node *end = parse_expr(parser);
          expecttok(parser, TOK_CSQUARE);
          return parse_access(
              parser, slice_node_new(left, key_or_start, end, sourcepos));
        }
      } else {
        expecttok(parser, TOK_CSQUARE);
        return parse_access(parser,
                            subscript_node_new(left, key_or_start, sourcepos));
      }
    }
  }
  return left;
}

static struct ast_node *parse_array_decl(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttok(parser, TOK_OSQUARE)) {
    struct vec *values = vec_new();
    while (!accepttok(parser, TOK_CSQUARE)) {
      vec_push(values, parse_expr(parser));
      accepttok(parser, TOK_COMMA);
    }
    return array_decl_node_new(values, sourcepos);
  } else {
    return parse_obj_decl(parser);
  }
}

static struct ast_node *parse_obj_decl(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();

  if (accepttok(parser, TOK_OBRACE)) {
    struct vec *keys = vec_new();
    struct vec *values = vec_new();
    while (!accepttok(parser, TOK_CBRACE)) {
      char *key;
      bool str = false;
      if (matchtok(parser, TOK_ID)) {
        key = clone_str(expecttok(parser, TOK_ID)->val);
      } else if (matchtok(parser, TOK_STRING)) {
        key = clone_str(expecttok(parser, TOK_STRING)->val);
        str = true;
      } else {
        // throw an error
        expecttok(parser, TOK_ID);
      }
      vec_push(keys, key);

      if (str) {
        expecttok(parser, TOK_COLON);
        vec_push(values, parse_expr(parser));
      } else if (accepttok(parser, TOK_COLON)) {
        vec_push(values, parse_expr(parser));
      } else {
        vec_push(values, id_node_new(clone_str(key), NULL, sourcepos));
      }
      accepttok(parser, TOK_COMMA);
    }
    return obj_decl_node_new(keys, values, sourcepos);
  } else {
    return parse_term(parser);
  }
}

static struct ast_node *parse_term(struct parser *parser) {
  struct sourcepos *sourcepos = CURRENT_SOURCEPOS();
  if (accepttok(parser, TOK_OPAREN)) {
    struct ast_node *expr = parse_expr(parser);
    expecttok(parser, TOK_CPAREN);
    return expr;
  } else if (matchtokv(parser, TOK_ID, "fn") ||
             matchtokv(parser, TOK_ID, "func")) {
    return parse_func(parser);
  } else if (matchtok(parser, TOK_ID)) {
    struct ast_node *type = NULL;
    char *id = clone_str(expecttok(parser, TOK_ID)->val);
    if (accepttok(parser, TOK_COLON) && !accepttokv(parser, TOK_ID, "any")) {
      type = parse_or(parser);
    }
    return id_node_new(id, type, sourcepos);
  } else if (matchtok(parser, TOK_NUM)) {
    char *a = expecttok(parser, TOK_NUM)->val;
    int alen = strlen(a);
    bool is_float = false;
    for (int i = 0; i < alen; ++i) {
      if (a[i] == '.') {
        is_float = true;
        break;
      }
    }
    if (is_float) {
      return num_node_new(is_float, 0, atof(a), sourcepos);
    } else {
      return num_node_new(is_float, atoi(a), 0, sourcepos);
    }
  } else if (matchtok(parser, TOK_STRING)) {
    return string_node_new(clone_str(expecttok(parser, TOK_STRING)->val),
                           sourcepos);
  } else {
    if (eof(parser)) {
      vm_raise(parser->vm, obj_compile_error_new("Unexpected end-of-file",
                                                 (lasttok(parser))->sourcepos));
      return NULL;
    } else {
      struct tok *tok = curtok(parser);
      struct strbuf *strbuf = strbuf_new();
      strbuf_append_str(strbuf, "Unexpected ");
      strbuf_append_str(strbuf, toktype_t_names[tok->type]);
      char *message = strbuf_done(strbuf);

      vm_raise(parser->vm,
               obj_compile_error_new(message, curtok(parser)->sourcepos));
      free(message);
    }
  }
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

  struct strbuf *strbuf = strbuf_new();

  strbuf_append_str(strbuf, "Expected ");
  strbuf_append_str(strbuf, toktype_t_names[type]);
  strbuf_append_str(strbuf, ", found ");
  strbuf_append_str(strbuf, toktype_t_names[curtok(parser)->type]);
  strbuf_append_str(strbuf, " \"");
  strbuf_append_str(strbuf, curtok(parser)->val);
  strbuf_append(strbuf, '"');

  char *message = strbuf_done(strbuf);
  vm_raise(parser->vm,
           obj_compile_error_new(message, curtok(parser)->sourcepos));
  free(message);
}

static struct tok *expecttokv(struct parser *parser, toktype_t type,
                              char *val) {
  if (matchtokv(parser, type, val)) {
    struct tok *ret = curtok(parser);
    parser->pos++;
    return ret;
  }

  struct strbuf *strbuf = strbuf_new();

  strbuf_append_str(strbuf, "Expected ");
  strbuf_append_str(strbuf, toktype_t_names[type]);
  strbuf_append_str(strbuf, ": \"");
  strbuf_append_str(strbuf, val);
  strbuf_append_str(strbuf, "\", found ");
  strbuf_append_str(strbuf, toktype_t_names[curtok(parser)->type]);
  strbuf_append_str(strbuf, " \"");
  strbuf_append_str(strbuf, curtok(parser)->val);
  strbuf_append(strbuf, '"');

  char *message = strbuf_done(strbuf);
  vm_raise(parser->vm,
           obj_compile_error_new(message, curtok(parser)->sourcepos));
  free(message);
}