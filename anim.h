#ifndef BBK_ANIM_H_
#define BBK_ANIM_H_

#include <stdlib.h>
#include <curses.h>

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

Anim *bb_newanim(int height, int width);
void bb_addframe(Anim *anim);
int bb_playanim(Anim *anim, int begin_y, int begin_x, int reversed);
void bb_delanim(Anim *anim);

#endif /* BBK_ANIM_H_ */
