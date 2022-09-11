
#ifndef _HASSIUM_H_
#define _HASSIUM_H_

#include <ds/hashmap.h>
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
  struct hashmap *modules;
  struct hashmap *module_values;
  struct vec *compiling;
  struct vec *freed_code_objs;
};

extern struct hassium_ctx hassium_ctx;

struct code_obj *compile_module(struct sourcefile *, struct vm *);
struct code_obj *compile_module_for_import(struct code_obj *, struct vec *,
                                           struct vm *);
bool is_compiling_module(char *);
struct obj *run_module(struct code_obj *, struct vm *, bool);
void run_file(char *);
void done();

char *from_to_path(struct vec *);

#endif