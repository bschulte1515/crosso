#include "lettercell.h"
#include "state.h"
#include "grid.h"

void State::updateSelectedCell(int x, int y)
{
    LetterCell *oldCell = dynamic_cast<LetterCell *>(selectedCell);
    LetterCell *newCell = dynamic_cast<LetterCell *>(grid->getCells()[x][y]);
    if (!oldCell || !newCell) return;
    oldCell->setHighlight(false);
    newCell->setHighlight(true);
    selectedCell = newCell;
}

void State::swapFillDirection()
{
    fillDirection =
        fillDirection == HORIZONTAL ? VERTICAL : HORIZONTAL;
}
