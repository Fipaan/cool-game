CC=cc
OPT=-O3
CFLAGS=-Wall -Wextra -ggdb -std=c11 $(OPT)
INCLUDES=-I./include
LIBS=-L./lib/ -lraylib -lm

game:
	$(CC) $(CFLAGS) $(INCLUDES) -o game ./src/game.c $(LIBS)
