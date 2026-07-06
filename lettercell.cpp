#include "lettercell.h"
#include "grid.h"
#include <iostream>

LetterCell::LetterCell(int inX, int inY, int inSize) : Cell(inX, inY, inSize) {}
LetterCell::LetterCell(int inX, int inY, int inSize, QChar letterIn) : Cell(inX, inY, inSize), letter(letterIn) {}

void LetterCell::draw(QPainter *painter)
{
    drawLetter(painter, Qt::black);
}

void LetterCell::drawLetter(QPainter *painter, QColor color)
{
    painter->setPen(color);
    QFont font = painter->font();
    font.setPointSize(Grid::letter_font_size);
    painter->setFont(font);

    painter->drawText(
        toRectWithOffset(0, Grid::letter_offset_y),
        Qt::AlignCenter,
        QString(letter)
    );
}

bool LetterCell::isBlack() { return false; }

void LetterCell::print() {
    std::cout << "(" << x << ", " << y << "): " << letter.toLatin1() << std::endl;
}
