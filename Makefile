CFLAGS = -Wall -Werror -std=c89 -O0 -g
CLINK = -lncurses

dist : bb.c
	$(CC) $(CFLAGS) bb.c $(CLINK) -o bb
