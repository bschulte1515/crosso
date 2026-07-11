#ifndef DIRECTION_H
#define DIRECTION_H

#define OPPOSITE_DIRECTION(direction) 							   \
        direction == WordDirection::ACROSS ? WordDirection::DOWN : \
                                             WordDirection::ACROSS

/* Used for describing movement around the board when using arrow keys */
enum class MoveDirection {
    UP,
    DOWN,
    LEFT,
    RIGHT
};

/*
 * Used by the state to describe direction of entire words and
 * when entering letters in the grid or describing words/clues.
 */
enum class WordDirection {
    ACROSS,
    DOWN
};

#endif // DIRECTION_H
