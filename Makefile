CC = cc

CFLAGS = -Wall -Wextra -Werror -g


all: term.c term.h
	$(CC) $(CFLAGS) -c term.c -o lert

clean:
	rm lert
