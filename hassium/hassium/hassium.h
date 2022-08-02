
#ifndef _HASSIUM_H_
#define _HASSIUM_H_

#include <ds/strbuf.h>
#include <ds/vec.h>
#include <emit.h>
#include <lexer.h>
#include <libgen.h>
#include <parser.h>
#include <stdio.h>
#include <util.h>
#include <vm.h>

struct hassium_ctx {
  char *prog_path;
};

extern struct hassium_ctx hassium_ctx;

struct code_obj *compile_module(char *);
struct code_obj *compile_module_for_import(struct vec *);
void run_module(struct code_obj *);
void run_string(char *);

#endif