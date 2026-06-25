#include "lettercell.h"
#include <iostream>

LetterCell::LetterCell(int inX, int inY, int inSize) : Cell(inX, inY, inSize) {}
LetterCell::LetterCell(int inX, int inY, int inSize, QChar letterIn) : Cell(inX, inY, inSize), letter(letterIn) {}

void LetterCell::draw(QPainter *painter)
{
    painter->fillRect(x * size, y * size, size, size, Qt::white);
    painter->setPen(Qt::black);
    painter->drawRect(x * size, y * size, size, size);

    QFont font = painter->font();
    font.setPointSize(16);
    painter->setFont(font);

    painter->drawText(x * size, y * size + LETTER_Y_OFFSET, size, size, Qt::AlignCenter, QString(letter));


    if (highlighted) {
        QColor highlightColor(0, 100, 255, 40);
        painter->fillRect(x * size, y * size, size, size, highlightColor);
    }
    if (selected) {
        QColor selectedColor(0, 50, 255, 50);
        painter->fillRect(x * size, y * size, size, size, selectedColor);
    }
}

bool LetterCell::isBlack() { return false; }

void LetterCell::print() {
    std::cout << "(" << x << ", " << y << "): " << letter.toLatin1() << std::endl;
}
