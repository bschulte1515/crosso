#include "grid.h"
#include "blackcell.h"
#include "lettercell.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <iostream>

Grid::Grid(QWidget *parent, EditorState *e, int g, int c)
    : QWidget{parent}, editor(e), gridSize(g), cellSize(c)
{
    // Add the width of the pen to the canvas size to fit in all the edges of the board
    this->setFixedSize(gridSize * cellSize + PEN_WIDTH, gridSize * cellSize + PEN_WIDTH);

    for (int i = 0; i < gridSize; ++i) {
        std::vector<Cell*> row;
        for (int j = 0; j < gridSize; ++j) {
            row.push_back(new LetterCell(i, j, cellSize, ' '));
        }
        cells.push_back(row);
    }

    selectedCell = cells[0][0];

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void Grid::paintEvent(QPaintEvent *event)
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
            cells[i][j]->draw(&painter);
        }
    }
}

void Grid::toggleCell(int x, int y)
{
    Cell *cell = cells[x][y];
    if (cell->isBlack()) {
        cells[x][y] = new LetterCell(x, y, cellSize, ' ');
    } else {
        cells[x][y] = new BlackCell(x, y, cellSize);
    }
}

void Grid::switchEditingMode()
{
    editor->switchEditingMode();
    LetterCell *cell = dynamic_cast<LetterCell *>(selectedCell);
    if (cell) { cell->setHighlight(false); }
}

void Grid::handleShortcut(QKeyEvent *event)
{
    if (event->key() == Qt::Key_M) {
        switchEditingMode();
    }
    else if (event->key() == Qt::Key_S) {
        saveToString();
    }
    else if (event->key() == Qt::Key_D) {
        moveDirection = !moveDirection;
    }
}

void Grid::moveSelectedCell()
{
    int newX = selectedCell->getX() + !moveDirection;
    int newY = selectedCell->getY() + moveDirection;
    LetterCell *oldCell = dynamic_cast<LetterCell *>(cells[selectedCell->getX()][selectedCell->getY()]);

    if ((moveDirection && newY < gridSize) || (!moveDirection && newX < gridSize)) {
        LetterCell *newCell = dynamic_cast<LetterCell *>(cells[newX][newY]);
        if (!oldCell || !newCell) return;
        newCell->setHighlight(true);
        oldCell->setHighlight(false);
        selectedCell = cells[newX][newY];
    }
}

void Grid::mousePressEvent(QMouseEvent *event)
{
    int x = event->position().x() / cellSize;
    int y = event->position().y() / cellSize;

    if (editor->isEditingGrid()) {
        bool symmetricGrid = true;

        toggleCell(x, y);

        if (symmetricGrid) {
            toggleCell(gridSize-x-1, gridSize-y-1);
        }


    } else {
        // TODO: Maybe (looking at next TODO as well) add new function "updateSelectedCell"
        LetterCell *oldCell = dynamic_cast<LetterCell *>(selectedCell);
        if (oldCell) { oldCell->setHighlight(false); }

        LetterCell *newCell = dynamic_cast<LetterCell *>(cells[x][y]);
        if (newCell) { newCell->setHighlight(true); selectedCell = newCell; }
    }

    update();
}

void Grid::keyPressEvent(QKeyEvent *event)
{
    QString text = event->text();

    if (event->modifiers() & Qt::ControlModifier) {
        handleShortcut(event);
    }

    else if (!text.isEmpty() && !editor->isEditingGrid()) {
        QChar ch = text.at(0).toUpper();
        if (ch.isLetter()) {
            LetterCell *cell = dynamic_cast<LetterCell *>(selectedCell);
            if (cell) {
                cell->setLetter(ch);
            }
            moveSelectedCell();
        }
    }

    update();
}

QString Grid::saveToString()
{
    std::string gridAsString = "BLAH";
    std::cout << gridAsString << std::endl;
    return QString("HEHE");
}

void Grid::loadFromString(QString gridString)
{
    return;
}
