CC=gcc
CFLAGS= -O2 -Wall

AKIO_O = tokenadt.o numdict.o lex.o parser.o repl.o

akio: $(AKIO_O)
	$(CC) -o akio $(AKIO_O)

.PHONY: clean
clean:
	-rm akio *.o
