#ifndef BBK_BOARD_H_
#define BBK_BOARD_H_

#include <stdlib.h>
#include <curses.h>

#include "anim.h"

typedef enum {DEFAULT, RED, GREEN, YELLOW, BLUE, CYAN, MAGENTA, WHITE} Color;

#define BB_NORMAL   COLOR_PAIR(DEFAULT)
#define BB_SYMBOL   COLOR_PAIR(YELLOW)
#define BB_LINE     A_BOLD
#define BB_SELECT   COLOR_PAIR(CYAN)
#define BB_RESULT   COLOR_PAIR(GREEN)
#define BB_EXTRA    COLOR_PAIR(RED)

typedef struct Node {
    int begin_y;
    int begin_x;
    Anim *anim;
    struct Node *prev;
    struct Node *next;
} Node;

typedef struct {
    int height;
    int width;
    Node *first;
    Node *last;
    int *cells;
} Board;

Board *bb_newboard(int height, int width);
void bb_addnode(Board *board, Anim *anim);
void bb_playboard(Board *board, int skip);
void bb_delboard(Board *board);

#endif /* BBK_BOARD_H_ */
