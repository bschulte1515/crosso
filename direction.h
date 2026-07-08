#ifndef DIRECTION_H
#define DIRECTION_H

/* Used for describing direction relative to other cells */
enum Direction {
    ABOVE,
    BELOW,
    LEFT,
    RIGHT
};

/* Used by the state to describe direction of entire words */
/* Used when entering letters in the grid or describing words/clues */
enum WordDirection {
    ACROSS,
    DOWN
};

#endif // DIRECTION_H
