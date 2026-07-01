#ifndef WORD_H
#define WORD_H

#include "direction.h"

struct Word {
    int startX;
    int startY;
    int length;
    Direction direction;
    int clueNumber;
    // TODO
    // QString clue;
};

#endif // WORD_H
