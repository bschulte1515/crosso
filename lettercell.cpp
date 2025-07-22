#include "lettercell.h"

LetterCell::LetterCell(int inX, int inY, int inSize, QChar letterIn) : Cell(inX, inY, inSize), letter(letterIn), highlighted(false) {}

void LetterCell::draw(QPainter *painter)
{
    painter->fillRect(x * size, y * size, size, size, Qt::white);
    painter->setPen(Qt::black);
    painter->drawRect(x * size, y * size, size, size);

    QFont font = painter->font();
    //font.setBold(true);
    font.setPointSize(16);
    painter->setFont(font);

    painter->drawText(x * size, y * size + LETTER_Y_OFFSET, size, size, Qt::AlignCenter, QString(letter));


    if (highlighted) {
        QColor highlightColor(0, 100, 255, 40);
        painter->fillRect(x * size, y * size, size, size, highlightColor);
    }
}

bool LetterCell::isBlack() { return false; }
