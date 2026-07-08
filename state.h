#ifndef STATE_H
#define STATE_H

#include <QString>
#include <QHash>
#include <QKeyEvent>

#include "lettercell.h"
#include "direction.h"
#include "action.h"

enum Mode {
    LAYOUT,
    FILL,
    CLUE
};

class Grid;

class State
{
public:
    State();

    /// TODO: When we allow the ability to change keybinds, we'll need to save them
    /// to a config file. When starting up, we would find this file and load it here
    // State(QFile *file)

    void setGrid(Grid *newGrid) { grid = newGrid; }
    Grid *getGrid() { return grid; }
    void setSelectedCell(LetterCell *cell) { selectedCell = cell; }
    LetterCell *getSelectedCell() { return selectedCell; }
    void moveAndSelectNewCell(Direction direction);
    void selectCell(Cell *cell);
    WordDirection getFillDirection() { return fillDirection; }
    void setMode(Mode newMode) { mode = newMode; }
    Mode getMode() { return mode; }
    void setCurrentFile(QString filename) { currentFile = filename; }
    QString getCurrentFile() { return currentFile; }

    void swapFillDirection();

    bool keyPressAction(QKeyEvent *event);
    Action getAction(QKeyEvent *event) const;
    bool handleAction(Action action);

private:
    /* The current grid */
    Grid *grid;

    /*
     * Mapping from key press to action. This can be updated by the user and init'd by file
     * Different hash maps represent sets of keybinds associated with modifiers (or no modifier)
     * (e.g. keymap_ctrl_mod for the Qt::ControlModifier)
     */
    QHash<Qt::Key, Action> keymap_no_mod;
    QHash<Qt::Key, Action> keymap_ctrl_mod;

    LetterCell *selectedCell; // Cell currently selected (letter cell filled when entering in FILL mode)
    WordDirection fillDirection = ACROSS; // Direction of selected word/direction moved when filled
    Mode mode = LAYOUT;
    QString currentFile;
};

#endif // STATE_H
