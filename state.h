#ifndef STATE_H
#define STATE_H

#include <QString>
#include <QHash>
#include <QKeyEvent>

#include "cell.h"
#include "direction.h"
#include "action.h"

/*
 * Modes pertaining to editing the crossword
 * - LAYOUT: Edit the layout of the grid (i.e. the pattern of the black cells)
 * - FILL: Input the letters into the letter cells/words
 * - CLUES: Write clues for the words
 */
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

    /**
     * @brief keyPressAction	Handle for a key press passed on by the grid
     * @param event 			The key press event
     * @return					Result of the action performed as a result of the event
     */
    bool keyPressAction(QKeyEvent *event);

    /**
     * @brief getAction		Translate an event to an action
     * @param event 		The key press event
     * @return 				The action produced by the event
     */
    Action getAction(QKeyEvent *event) const;

    /**
     * @brief handleAction		Handles an action
     * @param action 			The action to process
     * @return					Result of handled action. False indicates failure
     */
    bool handleAction(Action action);

    /**
     * @brief moveCursor	Moves the cursor over one cell
     * @param direction 	The direction to move the cursor
     * @param reverse		Move backwards along the direction
     */
    void moveCursor(WordDirection direction, bool reverse);

    /**
     * @brief moveCursor	Moves the cursor over one cell
     * @param direction 	The direction to move the cursor
     */
    void moveCursor(MoveDirection direction);

    /**
     * @brief moveCursorToNextWord
     */
    void moveCursorToNextWord();

    /**
     * @brief moveCursorToPreviousWord
     */
    void moveCursorToPreviousWord();

    /**
     * @brief        Moves the cursor to any cell
     * @param x      The x-coordinate of the cell to move to
     * @param y	     The y-coordinate of the cell to move to
     */
    void moveCursor(int x, int y);

    /**
     * @brief     Toggles the direction we are "actively" filling
     */
    void toggleActiveDirection(void);

    /**
     * @brief     Toggles the mode we are "actively" in
     */
    void toggleActiveMode(void);

    /* Getters */
    Grid *getGrid() { return grid; }
    Cell *getCursor() { return cursor; }
    WordDirection getActiveDirection() { return activeDirection; }
    Mode getActiveMode() { return activeMode; }
    int getActiveWord(void);
    QString getCurrentFile() { return currentFile; }

    /* Setters */
    void setGrid(Grid *newGrid) { grid = newGrid; }
    void setActiveDirection(WordDirection direction) { activeDirection = direction; }
    void setActiveMode(Mode mode);
    void setCurrentFile(QString filename) { currentFile = filename; }

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
    QHash<Qt::Key, Action> keymap_alt_mod;
    QHash<Qt::Key, Action> keymap_shift_mod;
    QHash<Qt::Key, Action> keymap_meta_mod;

    /* Cell that is currently selected by the "cursor". Will always be in the seleceted word */
    Cell *cursor;

    /* Direction the cursor will move when a letter is entered/deleted */
    WordDirection activeDirection = WordDirection::ACROSS;
    Mode activeMode = Mode::LAYOUT;

    QString currentFile;
};

#endif // STATE_H
