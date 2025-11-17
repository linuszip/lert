CC = cc

CFLAGS = -Wall -Wextra  -g


all: term.c term.h
	$(CC) $(CFLAGS) term.c -o lert

clean:
	rm lert
