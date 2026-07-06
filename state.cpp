#include "state.h"
#include "grid.h"

void State::swapFillDirection()
{
    if (editingMode != FILL) return;
    fillDirection =
        fillDirection == ACROSS ? DOWN : ACROSS;
}

/**
 * @brief Attempts to set the selected cell
 * @note Will note update selected cell if @p cell is not a LETTER
 * @param cell
 */
void State::selectCell(Cell *cell)
{
    if (!cell) return;
    LetterCell *letter = dynamic_cast<LetterCell *>(cell);
    if (letter) selectedCell = letter;
}
