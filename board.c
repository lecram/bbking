#include "board.h"

Board *
bb_newboard(int height, int width)
{
    Board *board = (Board *) malloc(sizeof(Board));
    board->height = height;
    board->width = width;
    board->first = board->last = (Node *) NULL;
    board->cells = (int *) calloc(height * width, sizeof(int));
    return board;
}

void
bb_addnode(Board *board, Anim *anim)
{
    int y;
    int x;
    int clean;
    int begin_y = 0;
    int begin_x = 0;
    int bheight = board->height;
    int bwidth = board->width;
    int aheight = anim->height;
    int awidth = anim->width;
    Node *node = (Node *) malloc(sizeof(Node));
    for (begin_y = 0; begin_y < bheight - aheight; begin_y++) {
        for (begin_x = 0; begin_x < bwidth - awidth; begin_x++) {
            clean = 1;
            for (y = begin_y; y < begin_y + aheight; y++) {
                for (x = begin_x; x < begin_x + awidth; x++) {
                    if (board->cells[y * bwidth + x]) {
                        clean = 0;
                        break;
                    }
                }
                if (!clean) break;
            }
            if (clean) break;
        }
        if (clean) break;
    }
    node->begin_y = begin_y;
    node->begin_x = begin_x;
    node->anim = anim;
    node->prev = board->last;
    node->next = (Node *) NULL;
    if (board->first == NULL)
        board->first = node;
    else
        board->last->next = node;
    board->last = node;
    for (y = begin_y; y < begin_y + aheight; y++) {
        for (x = begin_x; x < begin_x + awidth; x++) {
            board->cells[y * bwidth + x] = 1;
        }
    }
}

void
bb_playboard(Board *board, int skip)
{
    int delta = 1;
    WINDOW *win;
    Node *node;
    erase();
    node = board->first;
    while (skip--) {
        win = subwin(
          stdscr,
          node->anim->height,
          node->anim->width,
          node->begin_y,
          node->begin_x
        );
        touchwin(win);
        mvwin(node->anim->last->win, node->begin_y, node->begin_x);
        overwrite(node->anim->last->win, win);
        mvwin(node->anim->last->win, -node->begin_y, -node->begin_x);
        wrefresh(win);
        node = node->next;
        delwin(win);
    }
    while (node != NULL) {
        delta = bb_playanim(
          node->anim,
          node->begin_y,
          node->begin_x,
          delta < 0
        );
        node = (delta < 0) ? node->prev : node->next;
    }
}

void
bb_delboard(Board *board)
{
    Node *node;
    Node *temp_node;
    node = board->first;
    while (node != NULL) {
        temp_node = node->next;
        bb_delanim(node->anim);
        free(node);
        node = temp_node;
    }
    free(board->cells);
    free(board);
}
