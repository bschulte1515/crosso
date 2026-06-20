#ifndef STATE_H
#define STATE_H

#include <QString>

#include "lettercell.h"

class Grid;

enum Direction {
    HORIZONTAL,
    VERTICAL
};

enum EditingMode {
    LAYOUT,
    FILL,
    CLUES
};

class State
{
public:
    State() {}

    void setGrid(Grid *newGrid) { grid = newGrid; }
    Grid *getGrid() { return grid; }
    void setSelectedCell(LetterCell *cell) { selectedCell = cell; }
    LetterCell *getSelectedCell() { return selectedCell; }
    Direction getFillDirection() { return fillDirection; }
    void setEditingMode(EditingMode mode) { editingMode = mode; }
    EditingMode getEditingMode() { return editingMode; }
    void setCurrentFile(QString filename) { currentFile = filename; }
    QString getCurrentFile() { return currentFile; }

    void updateSelectedCell(int x, int y);
    void swapFillDirection();

private:
    /* The current grid */
    Grid *grid;

    /* Paramters for editing answers */
    LetterCell *selectedCell; // Cell currently highlighted when not in grid edit mode
    Direction fillDirection = HORIZONTAL; // Direction of highlighting/moving

    /* Parameters for editing grid */
    // TODO

    EditingMode editingMode = LAYOUT;
    QString currentFile;
};

#endif // STATE_H
