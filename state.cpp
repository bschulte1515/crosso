#include "lettercell.h"
#include "state.h"
#include "grid.h"

void State::updateSelectedCell()
{
    int oldX = selectedCell->getX();
    int oldY = selectedCell->getY();
    int deltaX = (fillDirection == HORIZONTAL &&
                  !(oldX == grid->getSize() - 1)) ? 1 : 0;
    int deltaY = (fillDirection == VERTICAL &&
                  !(oldY == grid->getSize() - 1)) ? 1 : 0;
    int newX = oldX + deltaX;
    int newY = oldY + deltaY;
    LetterCell *oldCell = dynamic_cast<LetterCell *>(grid->getCells()[oldX][oldY]);
    LetterCell *newCell = dynamic_cast<LetterCell *>(grid->getCells()[newX][newY]);
    if (!oldCell || !newCell) return;
    oldCell->setHighlight(false);
    newCell->setHighlight(true);
    selectedCell = grid->getCells()[newX][newY];
}

void State::swapFillDirection()
{
    fillDirection =
        fillDirection == HORIZONTAL ? VERTICAL : HORIZONTAL;
}
