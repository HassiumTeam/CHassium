TARGET   = bin/hassium

CC       = gcc
# compiling flags here
CFLAGS   = -c -std=c99 -Wswitch -I.

LINKER   = gcc
# linking flags here
LFLAGS   = -Wall -I. -lm

# change these to proper directories where each file should be
SRCDIR   = src
OBJDIR   = obj
LIBDIR   = lib
BINDIR   = bin

OBJECTS += src/main.o
OBJECTS += src/compiler.o
OBJECTS += src/lexer/tokenizer.o

INCLUDES += inc/compiler.h
INCLUDES += inc/lexer/tok_type.h
INCLUDES += inc/lexer/token.h
INCLUDES += inc/lexer/tokenizer.h
INCLUDES += inc/src_loc.h

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
	@bin/hassium bin/test.has
