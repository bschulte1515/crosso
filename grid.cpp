#include "grid.h"
#include "state.h"
#include "blackcell.h"
#include "lettercell.h"
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

    state->setSelectedCell(getCells()[0][0]);

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

Grid::Grid(QWidget *parent, State *stateIn, QString grid, int g, int c)
    : QWidget{parent}, state(stateIn), size(g), cellSize(c)

{
    this->setFixedSize(size * cellSize + PEN_WIDTH, size * cellSize + PEN_WIDTH);

    if (grid.size() != size * size) return; // TODO: Throw error when num of characters doesn't match grid size
    for (int i = 0; i < size; i++) {
        std::vector<Cell*> row;
        for (int j = 0; j < size; j++) {
            QChar ch = grid[(j*size + i)];
            if (ch == "#") {
                row.push_back(new BlackCell(i, j, cellSize));
            } else {
                if (ch == '_') ch = ' '; // Convert spaces
                row.push_back(new LetterCell(i, j, cellSize, ch));
            }
        }
        cells.push_back(row);
    }

    state->setSelectedCell(cells[0][0]);

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
    Cell *cell = cells[x][y];
    if (cell->isBlack()) {
        delete cell;
        cell = new LetterCell(x, y, cellSize, ' ');
    } else {
        delete cell;
        cell = new BlackCell(x, y, cellSize);
    }
    if (!symmetric) return;

    // We want to make sure the cells are the exact same now
    Cell *opposite_cell = cells[size-x-1][size-y-1];
    if (cell->isBlack()) {
        delete opposite_cell;
        opposite_cell = new BlackCell(size-x-1, size-y-1, cellSize);
    } else {
        delete opposite_cell;
        opposite_cell = new LetterCell(size-x-1, size-y-1, cellSize, ' ');
    }
}

void Grid::switchEditingMode()
{
    switch(state->getEditingMode()) {
    case LAYOUT: {
        state->setEditingMode(FILL);
        LetterCell *cell = dynamic_cast<LetterCell *>(state->getSelectedCell());
        if (cell) { cell->setHighlight(true); }
        break;
    }
    case FILL: {
        LetterCell *cell = dynamic_cast<LetterCell *>(state->getSelectedCell());
        if (cell) { cell->setHighlight(false); }
        state->setEditingMode(LAYOUT);
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

// TODO
// void Grid::updateHighlighting(LetterCell *oldCell, LetterCell *newCell)
// {
    // oldCell->setHighlight(false);
    // newCell->setHighlight(true);
// }

void Grid::updateSelectedCell(int x, int y)
{
    // It should only be possible to set the selected cell to a letter cell.
    // Attempts to select a black cell with be ignored here.
    LetterCell *oldCell = dynamic_cast<LetterCell *>(state->getSelectedCell());
    LetterCell *newCell = dynamic_cast<LetterCell *>(cells[x][y]);
    if (!oldCell | !newCell) return;
    state->setSelectedCell(newCell);
    oldCell->setHighlight(false);
    newCell->setHighlight(true);
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
                if (!cell) return grid; // TODO: Throw error that error parsing (shouldn't happen though)
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
                if (ch == '_') ch = ' '; // Convert spaces
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
    state->setSelectedCell(cells[0][0]);
}
