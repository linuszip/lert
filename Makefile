CC = cc

CFLAGS = -Wall -Wextra -g
// CFLAGS = -Wall -Wextra -O3


all: lert.c lert.h
	$(CC) $(CFLAGS) lert.c -o lert

install: all
	mv ./lert ~/.local/bin

clean:
	rm lert
