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
    bool isHighlighted() { return highlighted; }
    void setHighlight(bool newHighlighted) { highlighted = newHighlighted; }
    void setSelected(bool newSelected) { selected = newSelected; }
    bool isSelected() { return selected; }
    QChar getLetter() { return letter; }
    void setLetter(QChar ch) { letter = ch; }
    virtual void print() override;

private:
    QChar letter = ' ';
    bool highlighted = false;
    bool selected = false;

    // Letters are placed slightly too high in cells so this is the offset
    static constexpr int LETTER_Y_OFFSET = 2;
};

#endif // LETTERCELL_H
