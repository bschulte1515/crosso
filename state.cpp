#include "lettercell.h"
#include "state.h"
#include "grid.h"
#include <iostream>

void State::updateSelectedCell()
{
    int oldX = selectedCell->getX();
    int oldY = selectedCell->getY();
    std::cout << fillDirection << std::endl;
    std::cout << grid->getSize() << std::endl;
    std::cout << (fillDirection == HORIZONTAL);
    std::cout << (fillDirection == VERTICAL);
    std::cout << !(oldY == grid->getSize() - 1);
    std::cout << !(oldX == grid->getSize() - 1) << std::endl;
    int deltaX = (fillDirection == HORIZONTAL &&
                  !(oldX == grid->getSize() - 1)) ? 1 : 0;
    int deltaY = (fillDirection == VERTICAL &&
                  !(oldY == grid->getSize() - 1)) ? 1 : 0;
    int newX = oldX + deltaX;
    int newY = oldY + deltaY;
    std::cout << oldX << " " << oldY << " " << deltaX << " " << deltaY << std::endl;
    LetterCell *oldCell = dynamic_cast<LetterCell *>(grid->getCells()[oldX][oldY]);
    LetterCell *newCell = dynamic_cast<LetterCell *>(grid->getCells()[newX][newY]);
    if (!oldCell || !newCell) return;
    newCell->setHighlight(true);
    oldCell->setHighlight(false);
    selectedCell = grid->getCells()[newX][newY];
}

void State::swapFillDirection()
{
    fillDirection =
        fillDirection == HORIZONTAL ? VERTICAL : HORIZONTAL;
}
