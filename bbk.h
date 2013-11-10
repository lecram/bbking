#ifndef BBK_BBK_H_
#define BBK_BBK_H_

#include <stdlib.h>
#include <locale.h>
#include <curses.h>

#include "board.h"

void bb_init();
void bb_clean();
void input(char buffer[], int maxlen);

#endif /* BBK_BBK_H_ */
