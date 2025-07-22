#ifndef LETTERCELL_H
#define LETTERCELL_H

#include "cell.h"

class LetterCell : public Cell
{
public:
    LetterCell(int xIn, int yIn, int sizeIn, QChar letterIn);
    virtual void draw(QPainter *painter) override;
    virtual bool isBlack() override;
    bool isHighlighted() { return highlighted; }
    void setHighlight(bool newHighlighted) { highlighted = newHighlighted; }
    void setLetter(QChar ch) { letter = ch; }

private:
    QChar letter;
    bool highlighted;

    static constexpr int LETTER_Y_OFFSET = 2; // Letters are placed slightly too high in cells so this is the offset
};

#endif // LETTERCELL_H
