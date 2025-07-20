#include "builder.h"
#include <QPainter>

Builder::Builder(QWidget *parent, int g, int c)
    : QWidget{parent}, gridSize(g), cellSize(c)
{
    // Add the width of the pen to the canvas size to fit in all the edges of the board
    this->setFixedSize(gridSize * cellSize + PEN_WIDTH, gridSize * cellSize + PEN_WIDTH);
}

void Builder::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    painter.fillRect(rect(), Qt::white);

    QPen pen(Qt::black);
    pen.setWidth(PEN_WIDTH);
    pen.setCapStyle(Qt::SquareCap);
    painter.setPen(pen);

    for (int row = 0; row <= gridSize; row++)
    {
        painter.drawLine(0, row * cellSize, gridSize * cellSize, row * cellSize);
    }

    for (int col = 0; col <= gridSize; col++)
    {
        painter.drawLine(col * cellSize, 0, col * cellSize, gridSize * cellSize);
    }

}
