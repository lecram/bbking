#include "anim.h"

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
