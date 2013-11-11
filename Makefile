CLINK = -lncurses
CFLAGS = -Wall -Werror -std=c89
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CFLAGS += -O0 -g
else
    CFLAGS += -O2
endif

objects = anim.o board.o bbk.o

dist : bbk

bbk : $(objects)
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
