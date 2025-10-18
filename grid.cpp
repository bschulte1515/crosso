#include "grid.h"
#include "blackcell.h"
#include "lettercell.h"
#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

Grid::Grid(QWidget *parent, EditorState *stateIn, int g, int c)
    : QWidget{parent}, state(stateIn), gridSize(g), cellSize(c)
{
    // Add the width of the pen to the canvas size to fit in all the edges of the board
    this->setFixedSize(gridSize * cellSize + PEN_WIDTH, gridSize * cellSize + PEN_WIDTH);

    for (int i = 0; i < gridSize; i++) {
        std::vector<Cell*> row;
        for (int j = 0; j < gridSize; j++) {
            row.push_back(new LetterCell(i, j, cellSize, ' '));
        }
        cells.push_back(row);
    }

    selectedCell = cells[0][0];

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

Grid::Grid(QWidget *parent, EditorState *stateIn, QString grid, int g, int c)
    : QWidget{parent}, state(stateIn), gridSize(g), cellSize(c)

{
    this->setFixedSize(gridSize * cellSize + PEN_WIDTH, gridSize * cellSize + PEN_WIDTH);

    if (grid.size() != gridSize * gridSize) return; // TODO: Throw error when num of characters doesn't match grid size
    for (int i = 0; i < gridSize; i++) {
        std::vector<Cell*> row;
        for (int j = 0; j < gridSize; j++) {
            QChar ch = grid[(j*gridSize + i)];
            if (ch == "#") {
                row.push_back(new BlackCell(i, j, cellSize));
            } else {
                if (ch == '_') ch = ' '; // Convert spaces
                row.push_back(new LetterCell(i, j, cellSize, ch));
            }
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
    state->switchEditingMode();
    LetterCell *cell = dynamic_cast<LetterCell *>(selectedCell);
    if (cell) { cell->setHighlight(false); }
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
            moveDirection = !moveDirection;
            break;
        default:
            break;
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

    if (state->isEditingGrid()) {
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

    else if (!text.isEmpty() && !state->isEditingGrid()) {
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

QString Grid::gridToString()
{
    QString gridAsString = "";
    for (int i = 0; i < gridSize; i++) {
        for (int j = 0; j < gridSize; j++) {
            if (cells[j][i]->isBlack()) {
                gridAsString += "#";
            } else {
                LetterCell *cell = dynamic_cast<LetterCell *>(cells[j][i]);
                if (!cell) return gridAsString; // TODO: Throw error that error parsing (shouldn't happen though)
                QChar letter = cell->getLetter();
                if (letter == " ") letter = '_';
                gridAsString += letter;
            }
        }
    }
    return gridAsString;
}

void Grid::stringToGrid(QString newGrid, int newGridSize)
{

    if (newGrid.size() != newGridSize * newGridSize) return; // TODO: Throw error when num of characters doesn't match grid size

    destroyGrid();
    gridSize = newGridSize;

    for (int i = 0; i < gridSize; i++) {
        std::vector<Cell *> row;
        for (int j = 0; j < gridSize; j++) {
            QChar ch = newGrid[(j*gridSize + i)];
            if (ch == "#") {
                row.push_back(new BlackCell(i, j, cellSize));
            } else {
                if (ch == '_') ch = ' '; // Convert spaces
                row.push_back(new LetterCell(i, j, cellSize, ch));
            }
        }
        cells.push_back(row);
    }

    this->setFixedSize(gridSize * cellSize + PEN_WIDTH, gridSize * cellSize + PEN_WIDTH);
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
    QString gridString = gridToString();
    out << gridSize << ',' << gridString;
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
    stringToGrid(items[1], size);
    selectedCell = cells[0][0];
}
