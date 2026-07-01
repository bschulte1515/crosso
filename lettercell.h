#ifndef LETTERCELL_H
#define LETTERCELL_H

#include "cell.h"

class LetterCell : public Cell
{
public:
    LetterCell(int xIn, int yIn, int sizeIn);
    LetterCell(int xIn, int yIn, int sizeIn, QChar letterIn);

    virtual void draw(QPainter *painter) override;
    virtual bool isBlack() override;
    QChar getLetter() { return letter; }
    void setLetter(QChar ch) { letter = ch; }
    virtual void print() override;

private:
    QChar letter = ' ';

    // Letters are placed slightly too high in cells so this is the offset
    static constexpr int LETTER_Y_OFFSET = 2;
};

#endif // LETTERCELL_H
