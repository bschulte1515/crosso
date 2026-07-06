#include "blackcell.h"
#include <iostream>

BlackCell::BlackCell(int inX, int inY, int inSize) : Cell(inX, inY, inSize) {}

void BlackCell::draw(QPainter *painter)
{
    painter->fillRect(
        toRect(),
        Qt::black
    );
}

bool BlackCell::isBlack() { return true; }

void BlackCell::print() {
    std::cout << "(" << x << ", " << y << "): BLACK" << std::endl;
}
