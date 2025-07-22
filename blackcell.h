#ifndef BLACKCELL_H
#define BLACKCELL_H

#include "cell.h"

class BlackCell : public Cell
{
public:
    BlackCell(int xIn, int yIn, int sizeIn);
    virtual void draw(QPainter *painter) override;
    virtual bool isBlack() override;
};

#endif // BLACKCELL_H
