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

    keymap_ctrl_mod[Qt::Key_M] = Action::STATE_SWITCH_MODE;
    keymap_ctrl_mod[Qt::Key_D] = Action::STATE_SWAP_FILL_DIRECTION;
    keymap_ctrl_mod[Qt::Key_S] = Action::SAVE_TO_FILE;
    keymap_ctrl_mod[Qt::Key_L] = Action::LOAD_FROM_FILE;
}

void State::swapFillDirection()
{
    if (mode != Mode::FILL) return;
    fillDirection =
        fillDirection == WordDirection::ACROSS ? WordDirection::DOWN :
                                                 WordDirection::ACROSS;
}

/**
 * @brief Attempts to set the selected cell
 * @note Will not update selected cell if @p cell is not a LETTER
 * @param cell
 */
void State::selectCell(Cell *cell)
{
    if (!cell) return;
    LetterCell *letter = dynamic_cast<LetterCell *>(cell);
    if (letter) selectedCell = letter;
}

/************************************
   Function associated with actions
 ************************************/

void State::moveAndSelectNewCell(Direction direction)
{
    Cell *cell = grid->getAdjacentCell(selectedCell, direction);
    if (!cell) return;
    if ((direction == Direction::ABOVE || direction == Direction::BELOW) &&
       fillDirection == WordDirection::ACROSS) {
        fillDirection = WordDirection::DOWN;
    } else if ((direction == Direction::LEFT || direction == Direction::RIGHT) &&
       fillDirection == WordDirection::DOWN) {
        fillDirection = WordDirection::ACROSS;
    } else {
        selectCell(cell);
    }
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
        moveAndSelectNewCell(Direction::ABOVE);
        break;
    case Action::GRID_MOVE_DOWN:
        moveAndSelectNewCell(Direction::BELOW);
        break;
    case Action::GRID_MOVE_LEFT:
        moveAndSelectNewCell(Direction::LEFT);
        break;
    case Action::GRID_MOVE_RIGHT:
        moveAndSelectNewCell(Direction::RIGHT);
        break;
    case Action::STATE_SWITCH_MODE:
        grid->switchMode();
        break;
    case Action::STATE_SWAP_FILL_DIRECTION:
        swapFillDirection();
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
