#include <string.h>

#include "bbk.h"

#define INPUT_LEN 256

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

void
bb_clean()
{
    endwin();
}

void
input(char buffer[], int maxlen)
{
    int i;

    nocbreak();
    echo();
    curs_set(TRUE);
    mvgetnstr(LINES - 2, 0, buffer, maxlen - 1);
    for (i = 0; i < COLS; i++)
        mvaddch(LINES - 2, i, ' ');
    cbreak();
    noecho();
    curs_set(FALSE);
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
    int i = 0;
    char a[INPUT_LEN];
    char b[INPUT_LEN];
    Board *board;

    bb_init();

    board = bb_newboard(LINES, COLS);

    while (1) {
        input(a, INPUT_LEN);
        input(b, INPUT_LEN);
        bb_addnode(board, bb_op_add(a, b));
        bb_playboard(board, i++);
    }

    bb_delboard(board);
    return 0;
}
