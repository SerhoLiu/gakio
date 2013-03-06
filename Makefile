CC = gcc
CFLAGS = -O2 -Wall

FAKIO_OBJ = tokenadt.o numdict.o lex.o parser.o repl.o

all:	fakio

fakio: $(FAKIO_OBJ)
	$(CC) $(CFLAGS)  $(FAKIO_OBJ) -o $@

.PHONY: clean
clean:
	-rm fakio *.o
