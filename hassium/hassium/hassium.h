
#ifndef _HASSIUM_H_
#define _HASSIUM_H_

#include <ds/vec.h>
#include <emit.h>
#include <lexer.h>
#include <parser.h>
#include <stdio.h>
#include <util.h>
#include <vm.h>

struct code_obj *compile_module(char *);
void run_module(struct code_obj *);
void run_string(char *);

#endif