#ifndef CELL_H
#define CELL_H

#include <QPainter>

class Cell
{
public:
    Cell(int inX, int inY, int inSize) : x(inX), y(inY), size(inSize) {}
    virtual ~Cell() {};

    virtual void draw(QPainter *painter) = 0;
    virtual bool isBlack() = 0;
    int getX() { return x; }
    int getY() { return y; }
    int getSize() { return size; }

protected:
    int x;
    int y;
    int size;

};

#endif // CELL_H
