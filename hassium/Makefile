
TARGET   = bin/hassium

CC       = gcc
# compiling flags here
CFLAGS   = -c -std=c99 -I hassium

LINKER   = gcc
# linking flags here
LFLAGS   = -Wall -I. -lm

OBJECTS += builtins/array.o
OBJECTS += builtins/bool.o
OBJECTS += builtins/builtin.o
OBJECTS += builtins/defaults.o
OBJECTS += builtins/func.o
OBJECTS += builtins/iter.o
OBJECTS += builtins/number.o
OBJECTS += builtins/string.o
OBJECTS += builtins/weakref.o
OBJECTS += ds/hashmap.o
OBJECTS += ds/intmap.o
OBJECTS += ds/strbuf.o
OBJECTS += ds/vec.o
OBJECTS += ast.o
OBJECTS += codeobj.o
OBJECTS += emit.o
OBJECTS += lexer.o
OBJECTS += main.o
OBJECTS += object.o
OBJECTS += parser.o
OBJECTS += util.o
OBJECTS += vm.o
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
	@bin/hassium "./bin/test.has"

.PHONY: test
test:
	@bin/hassium "./bin/tests.has"

.PHONY: leakcheck
leakcheck:
	@valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./bin/hassium ./bin/test.has

.PHONY: leakcheck_test
leakcheck_test:
	@valgrind --leak-check=full --show-leak-kinds=all ./bin/hassium ./bin/tests.has
