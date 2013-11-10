CFLAGS = -Wall -Werror -std=c89 -O0 -g
CLINK = -lncurses

objects = anim.o board.o bbk.o

dist : $(objects)
	$(CC) $(CFLAGS) $(objects) $(CLINK) -o bbk

anim.o : anim.h anim.c
	$(CC) -c $(CFLAGS) anim.c

board.o : anim.h board.h board.c
	$(CC) -c $(CFLAGS) board.c

bbk.o : anim.h board.h bbk.h bbk.c
	$(CC) -c $(CFLAGS) bbk.c

.PHONY : clean
clean :
	$(RM) $(objects)
