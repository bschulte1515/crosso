#include "builder.h"
#include "blackcell.h"
#include "lettercell.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

Builder::Builder(QWidget *parent, int g, int c)
    : QWidget{parent}, gridSize(g), cellSize(c)
{
    // Add the width of the pen to the canvas size to fit in all the edges of the board
    this->setFixedSize(gridSize * cellSize + PEN_WIDTH, gridSize * cellSize + PEN_WIDTH);

    for (int i = 0; i < gridSize; ++i) {
        std::vector<Cell*> row;
        for (int j = 0; j < gridSize; ++j) {
            row.push_back(new LetterCell(i, j, cellSize, ' '));
        }
        grid.push_back(row);
    }

    selectedCell = grid[0][0];

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
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

    for (int i = 0; i < gridSize; ++i) {
        for (int j = 0; j < gridSize; ++j) {
            grid[i][j]->draw(&painter);
        }
    }

}

void Builder::toggleCell(int x, int y)
{
    Cell *cell = grid[x][y];
    if (cell->isBlack()) {
        grid[x][y] = new LetterCell(x, y, cellSize, ' ');
    } else {
        grid[x][y] = new BlackCell(x, y, cellSize);
    }
}

void Builder::mousePressEvent(QMouseEvent *event)
{
    int x = event->position().x() / cellSize;
    int y = event->position().y() / cellSize;

    // TODO: Check that we are in edit mode
    if (isEditingGrid) {
        bool symmetricGrid = true;

        toggleCell(x, y);

        if (symmetricGrid) {
            toggleCell(gridSize-x-1, gridSize-y-1);
        }


    } else {
        // TODO: Maybe (looking at next TODO as well) add new function "updateSelectedCell"
        LetterCell *oldCell = dynamic_cast<LetterCell *>(selectedCell);
        if (oldCell) { oldCell->setHighlight(false); }

        LetterCell *newCell = dynamic_cast<LetterCell *>(grid[x][y]);
        if (newCell) { newCell->setHighlight(true); selectedCell = newCell; }
    }

    update();
}

void Builder::keyPressEvent(QKeyEvent *event)
{
    // TODO: Do some fixing to remove all these nested ifs (ugglyyy)
    QString text = event->text();
    if (!text.isEmpty()) {
        QChar ch = text.at(0).toUpper();
        if (ch.isLetter()) {
            LetterCell *oldCell = dynamic_cast<LetterCell *>(selectedCell);
            if (oldCell) {
                oldCell->setLetter(ch);
                if (selectedCell->getY() + 1 < gridSize) {
                    LetterCell *newCell = dynamic_cast<LetterCell *>(grid[selectedCell->getX()][selectedCell->getY() + 1]);
                    if (newCell) {
                        newCell->setHighlight(true);
                        oldCell->setHighlight(false);
                        selectedCell = grid[selectedCell->getX()][selectedCell->getY() + 1];
                    }
                }
                update();
            }
        }
    }
}
