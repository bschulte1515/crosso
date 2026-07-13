#include <QKeyEvent>
#include <QHash>

#include "state.h"
#include "grid.h"
#include "action.h"

State::State()
{
    keymap_no_mod[Qt::Key_Up] = Action::GRID_MOVE_UP;
    keymap_no_mod[Qt::Key_Down] = Action::GRID_MOVE_DOWN;
    keymap_no_mod[Qt::Key_Left] = Action::GRID_MOVE_LEFT;
    keymap_no_mod[Qt::Key_Right] = Action::GRID_MOVE_RIGHT;
    keymap_no_mod[Qt::Key_Backspace] = Action::GRID_REMOVE_LETTER;

    keymap_ctrl_mod[Qt::Key_Right] = Action::GRID_GOTO_NEXT_WORD;
    keymap_ctrl_mod[Qt::Key_Left] = Action::GRID_GOTO_PREVIOUS_WORD;
    keymap_ctrl_mod[Qt::Key_M] = Action::STATE_SWITCH_MODE;
    keymap_ctrl_mod[Qt::Key_D] = Action::STATE_TOGGLE_ACTIVE_DIRECTION;
    keymap_ctrl_mod[Qt::Key_S] = Action::SAVE_TO_FILE;
    keymap_ctrl_mod[Qt::Key_L] = Action::LOAD_FROM_FILE;
}

bool State::keyPressAction(QKeyEvent *event)
{
    Action action = getAction(event);
    if (action == Action::NO_ACTION) return false;
    return handleAction(action);
}

Action State::getAction(QKeyEvent *event) const
{
    Action action = Action::NO_ACTION;
    if (event->modifiers() == 0) {
        action = keymap_no_mod.value((Qt::Key)event->key(), Action::NO_ACTION);
    }
    else if (event->modifiers() & Qt::ControlModifier) {
        action = keymap_ctrl_mod.value((Qt::Key)event->key(), Action::NO_ACTION);
    }
    return action;
}

bool State::handleAction(Action action)
{
    switch (action) {
    case Action::GRID_MOVE_UP:
        moveCursor(MoveDirection::UP);
        break;
    case Action::GRID_MOVE_DOWN:
        moveCursor(MoveDirection::DOWN);
        break;
    case Action::GRID_MOVE_LEFT:
        moveCursor(MoveDirection::LEFT);
        break;
    case Action::GRID_MOVE_RIGHT:
        moveCursor(MoveDirection::RIGHT);
        break;
    case Action::GRID_REMOVE_LETTER:
        grid->removeLetter();
        break;
    case Action::GRID_GOTO_NEXT_WORD:
        moveCursorToNextWord();
        break;
    case Action::GRID_GOTO_PREVIOUS_WORD:
        moveCursorToPreviousWord();
        break;
    case Action::STATE_SWITCH_MODE:
        toggleActiveMode();
        break;
    case Action::STATE_TOGGLE_ACTIVE_DIRECTION:
        toggleActiveDirection();
        break;
    case Action::SAVE_TO_FILE:
        grid->saveToFile();
        break;
    case Action::LOAD_FROM_FILE:
        grid->loadFromFile();
        break;
    case Action::NO_ACTION:
    default:
        return false;
    }
    return true;
}

/**
 * @brief Move cursor in a ~word~ direction
 *
 * @note This particular function is used when entering or removing letters
 * 		 and thus active direction should not change.
 *
 * @param direction 	The direction to move in
 * @param reverse		Reverse the direction of movement (i.e. move backwards)
 */
void State::moveCursor(WordDirection direction, bool reverse)
{
    if (direction == WordDirection::ACROSS && !reverse) {
        moveCursor(MoveDirection::RIGHT);
    } else if (direction == WordDirection::ACROSS && reverse) {
        moveCursor(MoveDirection::LEFT);
    } else if (direction == WordDirection::DOWN && !reverse) {
        moveCursor(MoveDirection::DOWN);
    } else if (direction == WordDirection::DOWN && reverse) {
        moveCursor(MoveDirection::UP);
    }
}

/**
 * @brief Move cursor in a direction
 *
 * @note This particular function is used when moving around the board with
 *  	 (typically) arrow keys, so the active direction can be changed
 *
 * @param direction 	The direction to move in
 */
void State::moveCursor(MoveDirection direction)
{
    Cell *cell = nullptr;

    bool moveVertical =
        direction == MoveDirection::UP ||
        direction == MoveDirection::DOWN;

    bool moveHorizontal =
        direction == MoveDirection::LEFT ||
        direction == MoveDirection::RIGHT;

    if (moveVertical && activeDirection == WordDirection::ACROSS) {
        toggleActiveDirection();
        return;
    } else if (moveHorizontal && activeDirection == WordDirection::DOWN) {
        toggleActiveDirection();
        return;
    }

    cell = grid->getAdjacentCell(cursor, direction);
    if (!cell) return;
    moveCursor(cell->getX(), cell->getY());
}

void State::moveCursor(int x, int y)
{
    LetterCell *letter = nullptr;
    Cell *cell = grid->getCell(x, y);
    if (!cell) return;

    // If in LAYOUT, cursor can point to any cell. However,
    // in FILL, cursor can ~only~ point to a letter cell.
    if (activeMode == Mode::LAYOUT) {
        cursor = cell;
    } else if (activeMode == Mode::FILL && !cell->isBlack()) {
        letter = dynamic_cast<LetterCell *>(cell);
        assert(letter != nullptr);
        cursor = letter;
    }
}

void State::moveCursorToNextWord()
{
    if (activeMode != Mode::FILL) return;
    int wordIndex = getActiveWord();
    Word &word = grid->getNextWord(wordIndex);
    moveCursor(word.startX, word.startY);
}

void State::moveCursorToPreviousWord()
{
    if (activeMode != Mode::FILL) return;
    int wordIndex = getActiveWord();
    Word &word = grid->getPreviousWord(wordIndex);
    moveCursor(word.startX, word.startY);
}

void State::toggleActiveDirection()
{
    if (activeMode != Mode::FILL) return;
    if (activeDirection == WordDirection::ACROSS) {
        activeDirection = WordDirection::DOWN;
    } else {
        activeDirection = WordDirection::ACROSS;
    }
}

void State::toggleActiveMode()
{
    if (activeMode == Mode::LAYOUT) {
        setActiveMode(Mode::FILL);
    } else if (activeMode == Mode::FILL) {
        setActiveMode(Mode::LAYOUT);
    }
}

/**
 * @brief
 *
 * @invariant There MUST be a valid active word so we assert that the index cannot be -1
 *
 * @return
 */
int State::getActiveWord()
{
    int ret = 0;

    ret = grid->findWord(cursor->getX(), cursor->getY(), activeDirection);
    assert(ret != -1);

    return ret;
}

/**
 * @brief State::setActiveMode
 * @param newMode
 */
void State::setActiveMode(Mode newMode)
{
    LetterCell *letter = nullptr;

    if (newMode == Mode::LAYOUT) {
        // moveCursor(0, 0);
    } else if (newMode == Mode::FILL) {
        grid->refreshWords();
        letter = grid->getFirstLetter();
        assert(letter != NULL);
        moveCursor(letter->getX(), letter->getY());
    }
    activeMode = newMode;
}
