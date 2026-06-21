#include "grid.h"
#include "state.h"
#include "lettercell.h"
#include "blackcell.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

Grid::Grid(QWidget *parent, State *stateIn, int g, int c)
    : QWidget{parent}, state(stateIn), size(g), cellSize(c)
{
    // Add the width of the pen to the canvas size to fit in all the edges of the board
    this->setFixedSize(size * cellSize + PEN_WIDTH, size * cellSize + PEN_WIDTH);

    for (int i = 0; i < size; i++) {
        std::vector<Cell*> row;
        for (int j = 0; j < size; j++) {
            row.push_back(new LetterCell(i, j, cellSize, ' '));
        }
        cells.push_back(row);
    }
    LetterCell *cell = dynamic_cast<LetterCell *>(cells[0][0]);
    assert(cell != NULL);
    state->setSelectedCell(cell);

    setFocusPolicy(Qt::StrongFocus);
    setFocus();
}

void Grid::destroyGrid()
{
    for (auto &cellRow : cells) {
        for (auto *c: cellRow) {
            delete c;
        }
    }
    cells.clear();
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

    for (int row = 0; row <= size; row++)
    {
        painter.drawLine(0, row * cellSize, size * cellSize, row * cellSize);
    }

    for (int col = 0; col <= size; col++)
    {
        painter.drawLine(col * cellSize, 0, col * cellSize, size * cellSize);
    }

    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            cells[i][j]->draw(&painter);
        }
    }
}

void Grid::toggleCells(int x, int y, bool symmetric)
{
    bool wasBlack = cells[x][y]->isBlack();

    delete cells[x][y];
    if (wasBlack) {
        cells[x][y] = new LetterCell(x, y, cellSize);
    } else {
        cells[x][y] = new BlackCell(x, y, cellSize);
    }
    if (!symmetric) return;

    delete cells[size-x-1][size-y-1];
    if (wasBlack) {
        cells[size-x-1][size-y-1] = new LetterCell(size-x-1, size-y-1, cellSize);
    } else {
        cells[size-x-1][size-y-1] = new BlackCell(size-x-1, size-y-1, cellSize);
    }
}

void Grid::switchEditingMode()
{
    switch(state->getEditingMode()) {
    case LAYOUT: {
        state->setEditingMode(FILL);
        addHighlighting();
        break;
    }
    case FILL: {
        state->setEditingMode(LAYOUT);
        removeHighlighting();
        break;
    }
    case CLUES:
    default:
        break;
    }
}

void Grid::handleShortcut(QKeyEvent *event)
{
    switch (event->key()) {
        case Qt::Key_M:
            switchEditingMode();
            break;
        case Qt::Key_S:
            saveToFile();
            break;
        case Qt::Key_L:
            loadFromFile();
            break;
        case Qt::Key_D:
            state->swapFillDirection();
            break;
        default:
            break;
    }
}

void Grid::mousePressEvent(QMouseEvent *event)
{
    int x = event->position().x() / cellSize;
    int y = event->position().y() / cellSize;

    if (state->getEditingMode() == LAYOUT) {
        bool symmetricGrid = true;
        toggleCells(x, y, symmetricGrid);
    } else {
        updateSelectedCell(x, y);
    }

    update();
}

/**
 * @brief Grid::keyPressEvent   Gets called when the grid is focused and a key is pressed.
 * @param event                 The key press(es) that occured to trigger the event
 */
void Grid::keyPressEvent(QKeyEvent *event)
{
    QString text = event->text();

    // Process a shortcut
    if (event->modifiers() & Qt::ControlModifier) {
        handleShortcut(event);
        goto exit;
    }
    // Process a key press when we are filling the grid
    if (!text.isEmpty() && state->getEditingMode() == FILL) {
        QChar ch = text.at(0).toUpper();
        if (ch.isLetter()) {
            LetterCell *cell = dynamic_cast<LetterCell *>(state->getSelectedCell());
            if (cell) {
                cell->setLetter(ch);
            }
            int x = 0, y = 0;
            getNextCell(&x, &y);
            updateSelectedCell(x, y);
        }
    }

exit:
    update();
}

void Grid::getNextCell(int *x, int *y)
{
    int oldX = state->getSelectedCell()->getX();
    int oldY = state->getSelectedCell()->getY();
    int deltaX = (state->getFillDirection() == HORIZONTAL &&
                  !(oldX == getSize() - 1)) ? 1 : 0;
    int deltaY = (state->getFillDirection() == VERTICAL &&
                  !(oldY == getSize() - 1)) ? 1 : 0;
    *x = oldX + deltaX;
    *y = oldY + deltaY;
}

void Grid::addHighlighting()
{
    Cell *c = state->getSelectedCell();
    if (state->getFillDirection() == HORIZONTAL) {
        while (!c->isBlack() && c->getX() != 0) {
            c = cells[c->getX()-1][c->getY()];
            c->setHighlight(true);
        }
        c = state->getSelectedCell();
        while (!c->isBlack() && c->getY() != size - 1) {
            c = cells[c->getX()+1][c->getY()];
            c->setHighlight(true);
        }
    }
    else if (state->getFillDirection() == VERTICAL) {
        while (!c->isBlack() && c->getY() != 0) {
            c = cells[c->getX()][c->getY()-1];
            c->setHighlight(true);
        }
        c = state->getSelectedCell();
        while (!c->isBlack() && c->getY() != size - 1) {
            c = cells[c->getX()][c->getY()+1];
            c->setHighlight(true);
        }
    }
}

void Grid::removeHighlighting(LetterCell *cell)
{
    // Remove the highlighting for the row/column of the new selected cell
    if (state->getFillDirection() == HORIZONTAL) {
        for (int i = 0; i < size; i++) {
            cells[i][cell->getY()]->setHighlight(false);
        }
    }
    else if (state->getFillDirection() == VERTICAL) {
        for (int i = 0; i < size; i++) {
            cells[cell->getX()][i]->setHighlight(false);
        }
    }
    cell->setSelected(false);
}

void Grid::updateSelectedCell(int x, int y)
{
    // It should only be possible to set the selected cell to a letter cell.
    // Attempts to select a black cell with be ignored here.
    LetterCell *oldCell = dynamic_cast<LetterCell *>(state->getSelectedCell());
    LetterCell *newCell = dynamic_cast<LetterCell *>(cells[x][y]);
    if (!oldCell | !newCell) return;
    state->setSelectedCell(newCell);
    removeHighlighting(oldCell);
    addHighlighting();
}

QString Grid::toString()
{
    QString grid = "";
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (cells[j][i]->isBlack()) {
                grid += "#";
            } else {
                LetterCell *cell = dynamic_cast<LetterCell *>(cells[j][i]);
                assert(cell != NULL);
                QChar letter = cell->getLetter();
                if (letter == " ") letter = '_';
                grid += letter;
            }
        }
    }
    return grid;
}

void Grid::fromString(QString newGrid, int newSize)
{

    if (newGrid.size() != newSize * newSize) return; // TODO: Throw error when num of characters doesn't match grid size

    destroyGrid();
    size = newSize;

    for (int i = 0; i < size; i++) {
        std::vector<Cell *> row;
        for (int j = 0; j < size; j++) {
            QChar ch = newGrid[(j*size + i)];
            if (ch == "#") {
                row.push_back(new BlackCell(i, j, cellSize));
            } else {
                row.push_back(new LetterCell(i, j, cellSize, ch));
            }
        }
        cells.push_back(row);
    }

    this->setFixedSize(size * cellSize + PEN_WIDTH, size * cellSize + PEN_WIDTH);
    emit gridResized();
    update();
}

void Grid::saveToFile()
{
    QString filename;
    if (state->getCurrentFile().isEmpty()) {
        filename = QFileDialog::getSaveFileName(this, "Save");
        if (filename.isEmpty()) {
            return;
        }
        state->setCurrentFile(filename);
    }

    QFile file(state->getCurrentFile());
    if (!file.open(QIODevice::WriteOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot save file: " + file.errorString());
        return;
    }
    setWindowTitle(state->getCurrentFile());
    QTextStream out(&file);
    QString gridString = this->toString();
    out << size << ',' << gridString;
    file.close();
}

void Grid::loadFromFile()
{
    QString filename = QFileDialog::getOpenFileName(this, "Open the file");
    if (filename.isEmpty()) {
        return;
    }
    QFile file(filename);
    state->setCurrentFile(filename);
    if (!file.open(QIODevice::ReadOnly | QFile::Text)) {
        QMessageBox::warning(this, "Warning", "Cannot open file: " + file.errorString());
        return;
    }
    setWindowTitle(filename);
    QTextStream in(&file);
    QString text = in.readAll();
    file.close();

    QStringList items = text.split(',');
    if (items.length() != 2) return;
    bool ok;
    int size = items[0].toInt(&ok);
    if (!ok) return;
    this->fromString(items[1], size);
    // TODO: This will *BREAK* (selected cell could be null, which is not expected)
    //		 Add a "getFirstLetterCell" that returns the upper-leftmost LetterCell
    //		 Would also need to ensure that grid is not completely black (>= 1 letter)
    LetterCell *cell = dynamic_cast<LetterCell *>(cells[0][0]);
    if (!cell) return;
    state->setSelectedCell(cell);
}

void Grid::updateRows()
{
    for (int x = 0; x < size; x ++) {
        bool insideRow = false;
        int rowStart = -1;
        for (int y = 0; y < size; y ++) {
            // If we hit a black cell and we were inside a row,
            // save this off as a row
            if (c->isBlack() && insideRow) {

            }
            // If we hit a black cell and we were not inside a row,
            // (i.e. previous cell was a letter or this is the first black cell,
            else if (c->isBlack() && !insideRow) {
                continue;
            }
            else if (!c->isBlack() && insideRow) {

            }
            else if (!c->isBlack() && insideRow) {

            }
        }
    }
}

void Grid::updateCols()
{
    for (auto &cellRow : cells) {
        for (auto *c: cellRow) {
            delete c;
        }
    }
}
