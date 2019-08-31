TARGET   = bin/hassium

CC       = gcc
# compiling flags here
CFLAGS   = -c -std=c99 -I inc

LINKER   = gcc
# linking flags here
LFLAGS   = -Wall -I. -lm

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
LIBDIR   = lib
BINDIR   = bin

OBJECTS += src/emit/emit.o
OBJECTS += src/emit/inst.o
OBJECTS += src/emit/symbol_table.o

OBJECTS += src/has_lib/modules/default/default_mod.o
OBJECTS += src/has_lib/modules/default/iter.o
OBJECTS += src/has_lib/modules/default/list.o
OBJECTS += src/has_lib/modules/default/number.o
OBJECTS += src/has_lib/modules/default/string.o

OBJECTS += src/has_lib/has_bound_func.o
OBJECTS += src/has_lib/has_class.o
OBJECTS += src/has_lib/has_func.o
OBJECTS += src/has_lib/has_method.o
OBJECTS += src/has_lib/has_obj.o
OBJECTS += src/has_lib/has_obj_lib.o
OBJECTS += src/has_lib/has_type.o

OBJECTS += src/lexer/token.o
OBJECTS += src/lexer/tokenizer.o

OBJECTS += src/parser/ast.o
OBJECTS += src/parser/parser.o

OBJECTS += src/util/dict.o
OBJECTS += src/util/int_dict.o
OBJECTS += src/util/int_vector.o
OBJECTS += src/util/vector.o

OBJECTS += src/vm/gc.o
OBJECTS += src/vm/stack_frame.o
OBJECTS += src/vm/vm.o

OBJECTS += src/main.o
OBJECTS += src/compiler.o

INCLUDES += inc/emit/emit.h
INCLUDES += inc/emit/inst.h
INCLUDES += inc/emit/symbol_table.h

INCLUDES += inc/has_lib/modules/default/default_mod.h
INCLUDES += inc/has_lib/modules/default/iter.h
INCLUDES += inc/has_lib/modules/default/list.h
INCLUDES += inc/has_lib/modules/default/number.h
INCLUDES += inc/has_lib/modules/default/string.h

INCLUDES += inc/has_lib/has_bound_func.h
INCLUDES += inc/has_lib/has_class.h
INCLUDES += inc/has_lib/has_func.h
INCLUDES += inc/has_lib/has_method.h
INCLUDES += inc/has_lib/has_obj.h
INCLUDES += inc/has_lib/has_obj_lib.h
INCLUDES == inc/has_lib/has_type.h

INCLUDES += inc/lexer/token.h
INCLUDES += inc/lexer/tokenizer.h

INCLUDES += inc/parser/ast.h
INCLUDES += inc/parser/node_type.h
INCLUDES += inc/parser/parser.h

INCLUDES += inc/util/dict.h
INCLUDES += inc/util/int_dict.h
INCLUDES += inc/util/int_vector.h
INCLUDES += inc/util/vector.h

INCLUDES += inc/vm/gc.h
INCLUDES += inc/vm/stack_frame.h
INCLUDES += inc/vm/vm.h

INCLUDES += inc/bin_op_type.h
INCLUDES += inc/compiler.h
INCLUDES += inc/func_param.h
INCLUDES += inc/src_loc.h
INCLUDES += inc/unary_op_type.h

rm       = rm -f


$(TARGET): $(OBJECTS)
	$(LINKER) -o $@ $(LFLAGS) $^ -lgcc
%.o: %.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(TARGET)
	@echo "Executable removed!"

.PHONY: run
run:
	@bin/hassium

.PHONY: leakcheck
leakcheck:
	@valgrind --leak-check=full --show-leak-kinds=all ./bin/hassium
