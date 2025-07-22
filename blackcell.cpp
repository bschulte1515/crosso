#include "blackcell.h"

BlackCell::BlackCell(int inX, int inY, int inSize) : Cell(inX, inY, inSize) {}

void BlackCell::draw(QPainter *painter)
{
    painter->fillRect(x * size, y * size, size, size, Qt::black);
    painter->setPen(Qt::black);
    painter->drawRect(x * size, y * size, size, size);
}

bool BlackCell::isBlack() { return true; }
