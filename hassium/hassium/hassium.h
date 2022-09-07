
#ifndef _HASSIUM_H_
#define _HASSIUM_H_

#include <ds/strbuf.h>
#include <ds/vec.h>
#include <emit.h>
#include <lexer.h>
#include <libgen.h>
#include <object.h>
#include <parser.h>
#include <sourcefile.h>
#include <stdio.h>
#include <util.h>
#include <vm.h>

struct hassium_ctx {
  char *prog_path;
};

extern struct hassium_ctx hassium_ctx;

struct code_obj *compile_module(struct sourcefile *, struct vm *);
struct code_obj *compile_module_for_import(struct vec *, struct vm *);
void run_module(struct code_obj *, struct vm *);
void run_file(char *);

#endif