TARGET   = bin/hassium

CC       = gcc
# compiling flags here
CFLAGS   = -c -std=c99 -I inc

LINKER   = gcc
# linking flags here
LFLAGS   = -Wall -I. -lm

OBJECTS += src/lexer/lexer.o
OBJECTS += src/lexer/token.o

OBJECTS += src/main.o

OBJECTS += src/util/string.o
OBJECTS += src/util/vector.o

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
	@valgrind --leak-check=full --show-leak-kinds=all ./bin/hassium ./bin/test.has

.PHONY: leakcheck_test
leakcheck_test:
	@valgrind --leak-check=full --show-leak-kinds=all ./bin/hassium ./bin/tests.has
