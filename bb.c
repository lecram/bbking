#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <curses.h>

#define MIN(A, B) ((A) < (B)) ? (A) : (B)
#define MAX(A, B) ((A) > (B)) ? (A) : (B)

typedef enum {DEFAULT, RED, GREEN, YELLOW, BLUE, CYAN, MAGENTA, WHITE} Color;

#define BB_NORMAL   COLOR_PAIR(DEFAULT)
#define BB_SYMBOL   COLOR_PAIR(YELLOW)
#define BB_LINE     A_BOLD
#define BB_SELECT   COLOR_PAIR(CYAN)
#define BB_RESULT   COLOR_PAIR(GREEN)
#define BB_EXTRA    COLOR_PAIR(RED)

typedef struct Frame {
    WINDOW *win;
    struct Frame *prev;
    struct Frame *next;
} Frame;

typedef struct {
    int height;
    int width;
    WINDOW *win;
    Frame *first;
    Frame *last;
} Anim;

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

Anim *
bb_newanim(int height, int width)
{
    Anim *anim = (Anim *) malloc(sizeof(Anim));
    anim->height = height;
    anim->width = width;
    anim->win = newwin(height, width, 0, 0);
    anim->first = anim->last = (Frame *) NULL;
    return anim;
}

void
bb_addframe(Anim *anim)
{
    Frame *frame = (Frame *) malloc(sizeof(Frame));
    frame->win = anim->win;
    frame->prev = anim->last;
    frame->next = (Frame *) NULL;
    if (anim->first == NULL)
        anim->first = frame;
    else
        anim->last->next = frame;
    anim->last = frame;
    anim->win = dupwin(anim->win);
}

int
bb_playanim(Anim *anim, int begin_y, int begin_x, int reversed)
{
    int delta = reversed ? -1 : +1;
    Frame *frame = reversed ? anim->last : anim->first;
    WINDOW *win = subwin(stdscr, anim->height, anim->width, begin_y, begin_x);
    touchwin(win);
    while (frame != NULL) {
        mvwin(frame->win, begin_y, begin_x);
        overwrite(frame->win, win);
        mvwin(frame->win, -begin_y, -begin_x);
        wrefresh(win);
        switch (getch()) {
            case KEY_LEFT:
                frame = frame->prev;
                delta = -1;
                break;
            case KEY_RIGHT:
                frame = frame->next;
                delta = +1;
                break;
            case ' ':
                if (delta > 0)
                    frame = (frame == anim->last) ? NULL: anim->last;
                else
                    frame = (frame == anim->first) ? NULL: anim->first;
                break;
        }
    }
    delwin(win);
    return delta;
}

void
bb_delanim(Anim *anim)
{
    Frame *frame;
    Frame *temp_frame;
    frame = anim->first;
    while (frame != NULL) {
        temp_frame = frame->next;
        delwin(frame->win);
        free(frame);
        frame = temp_frame;
    }
    delwin(anim->win);
    free(anim);
}

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
bb_playboard(Board *board)
{
    int delta = 1;
    Node *node = board->first;
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

void
bb_clean()
{
    endwin();
}

void
bb_init()
{
    setlocale(LC_ALL, "");

    initscr();

    cbreak(); /* Get one character at a time. */
    noecho();

    nonl(); /* No NL->CR/NL on output. */
    intrflush(stdscr, FALSE);
    keypad(stdscr, TRUE);

    curs_set(FALSE); /* Hide blinking cursor. */

    if (has_colors()) {
        start_color();
        init_pair(RED, COLOR_RED,     COLOR_BLACK);
        init_pair(GREEN, COLOR_GREEN,   COLOR_BLACK);
        init_pair(YELLOW, COLOR_YELLOW,  COLOR_BLACK);
        init_pair(BLUE, COLOR_BLUE,    COLOR_BLACK);
        init_pair(CYAN, COLOR_CYAN,    COLOR_BLACK);
        init_pair(MAGENTA, COLOR_MAGENTA, COLOR_BLACK);
        init_pair(WHITE, COLOR_WHITE,   COLOR_BLACK);
    }
    atexit(bb_clean);
}

Anim *
bb_op_add(char *a, char *b)
{
    Anim *anim;
    char *t = NULL;
    int i;
    int carry;
    int oldcarry;
    int digit;
    int maxlen;
    int lena = strlen(a);
    int lenb = strlen(b);
    if (lena > lenb) {
        maxlen = lena;
        t = (char *) malloc((lena + 1) * sizeof(char));
        for (i = 0; i < lena - lenb; i++)
            t[i] = ' ';
        strcpy(t + i, b);
        b = t;
    }
    else if (lena < lenb) {
        maxlen = lenb;
        t = (char *) malloc((lenb + 1) * sizeof(char));
        for (i = 0; i < lenb - lena; i++)
            t[i] = ' ';
        strcpy(t + i, a);
        a = t;
    }
    else
        maxlen = lena; /* == lenb */
    anim = bb_newanim(7, maxlen + 5);
    bb_addframe(anim);
    mvwaddstr(anim->win, 2, 2, a);
    mvwaddstr(anim->win, 3, 2, b);
    for (i = 0; i < maxlen + 1; i++)
        mvwaddch(anim->win, 4, i + 1, ACS_HLINE | BB_LINE);
    mvwaddch(anim->win, 3, maxlen + 3, '+' | BB_SYMBOL);
    bb_addframe(anim);
    carry = 0;
    for (i = maxlen - 1; i >= 0; i--) {
        digit = carry;
        if (carry)
            mvwaddch(anim->win, 1, i + 2, '1' | BB_SELECT);
        if (a[i] != ' ') {
            digit += a[i] - '0';
            mvwaddch(anim->win, 2, i + 2, a[i] | BB_SELECT);
        }
        if (b[i] != ' ') {
            digit += b[i] - '0';
            mvwaddch(anim->win, 3, i + 2, b[i] | BB_SELECT);
        }
        bb_addframe(anim);
        oldcarry = carry;
        if (digit >= 10) {
            carry = 1;
            mvwaddch(anim->win, 1, i + 1, '1' | BB_EXTRA);
            digit -= 10;
        }
        else carry = 0;
        mvwaddch(anim->win, 5, i + 2, (digit + '0') | BB_RESULT);
        bb_addframe(anim);
        if (oldcarry)
            mvwaddch(anim->win, 1, i + 2, '1' | BB_NORMAL);
        if (a[i] != ' ')
            mvwaddch(anim->win, 2, i + 2, a[i] | BB_NORMAL);
        if (b[i] != ' ')
            mvwaddch(anim->win, 3, i + 2, b[i] | BB_NORMAL);
    }
    if (carry) {
        mvwaddch(anim->win, 1, 1, '1' | BB_SELECT);
        bb_addframe(anim);
        mvwaddch(anim->win, 5, 1, '1' | BB_RESULT);
        bb_addframe(anim);
        mvwaddch(anim->win, 1, 1, '1' | BB_NORMAL);
    }
    bb_addframe(anim);
    if (t != NULL) free(t);
    return anim;
}

int
main(int argc, char *argv[])
{
    Board *board;

    bb_init();

    board = bb_newboard(LINES, COLS);

    bb_addnode(board, bb_op_add("123", "456"));
    bb_addnode(board, bb_op_add("999", "999"));
    bb_addnode(board, bb_op_add("789", "103452"));

    bb_playboard(board);

    bb_delboard(board);
    return 0;
}
