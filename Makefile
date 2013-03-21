CC = gcc
CFLAGS = -O0 -g -Wall

GAKIO_OBJ = tokenadt.o numdict.o vpool.o lex.o parser.o repl.o

all:	gakio

gakio: $(GAKIO_OBJ)
	$(CC) $(CFLAGS)  $(GAKIO_OBJ) -o $@

.PHONY: clean
clean:
	-rm gakio *.o
