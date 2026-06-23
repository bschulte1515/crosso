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
    resetGrid();

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

void Grid::resetGrid()
{
    updateWords();
    LetterCell *firstLetter = getFirstLetter();
    assert(firstLetter != NULL);
    state->setSelectedCell(firstLetter);
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
        resetGrid();
        addHighlighting();
        break;
    }
    case FILL: {
        state->setEditingMode(LAYOUT);
        removeHighlighting(state->getSelectedCell());
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
            Cell *next = getNextCell(state->getSelectedCell(), state->getFillDirection());
            if (next) updateSelectedCell(next->getX(), next->getY());
        }
    }

exit:
    update();
}

LetterCell *Grid::getFirstLetter()
{
    if (words.empty()) return NULL;
    struct Word firstWord = words[0];
    Cell *firstCell = cells[firstWord.startX][firstWord.startY];
    return dynamic_cast<LetterCell *>(firstCell);
}

/**
 * @brief Grid::getNextCell      Returns a pointer to the "next" cell
 * The next cell will either be the cell directly right or down, based on direction
 * @param cell					 The cell to start at
 * @param direction				 The direction in which to traverse the cells
 * @return 						 The next cell. Set to NULL if traversing would lead to OOB
 */
Cell *Grid::getNextCell(Cell *cell, Direction direction)
{
    int oldX = cell->getX();
    int oldY = cell->getY();
    int deltaX = (direction == ACROSS &&
                  !(oldX == size - 1)) ? 1 : 0;
    int deltaY = (direction == DOWN &&
                  !(oldY == size - 1)) ? 1 : 0;
    if (deltaX == 0 && deltaY == 0) return NULL;
    return cells[oldX + deltaX][oldY + deltaY];
}

void Grid::addHighlighting()
{
    LetterCell *cell = state->getSelectedCell();
    cell->setSelected(true);

    // Remove the highlighting for the row/column of the new selected cell
    int word_index = findWord(cell->getX(), cell->getY(), state->getFillDirection());
    if (word_index == -1) {
        std::cout << __func__ << ":" << __LINE__ << " Couldn't find word!" << std::endl;
        return;
    }
    auto &word = words[word_index];
    int offset = 0;
    LetterCell *currentCell = cell;
    while (currentCell && offset < word.length) {
        currentCell->setHighlight(false);
        offset++;
        if (word.direction == ACROSS) {
            currentCell = dynamic_cast<LetterCell *>(cells[word.startX + offset][word.startY]);
        }
        else if (word.direction == DOWN) {
            currentCell = dynamic_cast<LetterCell *>(cells[word.startX][word.startY + offset]);
        }
    }
}

void Grid::removeHighlighting(LetterCell *cell)
{
    cell->setSelected(false);

    // Remove the highlighting for the row/column of the new selected cell
    int word_index = findWord(cell->getX(), cell->getY(), state->getFillDirection());
    if (word_index == -1) {
        std::cout << __func__ << ":" << __LINE__ << " Couldn't find word!" << std::endl;
        return;
    }
    auto &word = words[word_index];
    int offset = 0;
    LetterCell *currentCell = cell;
    while (currentCell && offset < word.length) {
        currentCell->setHighlight(false);
        offset++;
        if (word.direction == ACROSS) {
            currentCell = dynamic_cast<LetterCell *>(cells[word.startX + offset][word.startY]);
        }
        else if (word.direction == DOWN) {
            currentCell = dynamic_cast<LetterCell *>(cells[word.startX][word.startY + offset]);
        }
    }
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
    resetGrid();
}

/**
 * @brief Grid::startsWord		Helper function that checks if this cell should be the start of a word
 * @param cell 					The cell to check
 * @param direction 			The direction to check
 * @return						Whether the cell is the start of a word in the direction given
 */
bool Grid::startsWord(Cell *cell, Direction direction)
{
    if (direction == ACROSS) {
        if (cell->getX() == 0) return true;
        Cell *cellLeft = cells[cell->getX()-1][cell->getY()];
        if (cellLeft->isBlack()) return true;
    }
    else if (direction == DOWN) {
        if (cell->getY() == 0) return true;
        Cell *cellAbove = cells[cell->getX()][cell->getY()-1];
        if (cellAbove->isBlack()) return true;
    }
    return false;
}

/**
 * @brief Grid::parseWord		Parses the word starting with @ref cell
 * 								@note Assumes that this Cell is the start of the word
 * @param cell
 * @param direction
 * @return
 */
struct Word Grid::parseWord(Cell *cell, Direction direction)
{
    struct Word newWord = {
        .startX = cell->getX(),
        .startY = cell->getY(),
        .length = 0,
        .direction = direction
    };
    for (Cell *next = cell;
         next != NULL;
         next = getNextCell(next, direction))
    {
        newWord.length++;
    }
    return newWord;
}

int Grid::findWord(int x, int y, Direction direction)
{
    for (unsigned int i = 0; i < words.size(); i++) {
        auto &word = words[i];
        if (direction == ACROSS &&
           word.startX <= x &&
           x <= (word.startX + word.length - 1)) {
            return i;
        }
        else if (direction == DOWN &&
                word.startY <= y &&
                y <= (word.startY + word.length - 1)) {
            return i;
        }
    }
    return -1;
}

/**
 * @brief Grid::updateWords      Parses the grid into words, first across then down
 *
 * Called everytime there is an update to the grid. For right now this should only be when switching
 * from LAYOUT -> FILL mode, or after initalizing the grid (regularly or fromString)
 */
void Grid::updateWords()
{
    words.clear();
    for (auto &row : cells) {
        for (auto *cell: row) {
            if (startsWord(cell, ACROSS)) {
                words.push_back(parseWord(cell, ACROSS));
            }
            if (startsWord(cell, DOWN)) {
                words.push_back(parseWord(cell, DOWN));
            }
        }
    }
    printWords();
}

void Grid::printWords()
{
    std::cout << "Words:" << std::endl;
    for (auto &elem : words) {
        std::cout << "(" << elem.startX << ", " << elem.startY << ") -> (";
        int newX = elem.startX;
        int newY = elem.startY;
        if (elem.direction == ACROSS) {
            newX += elem.length;
            newX--;
        }
        if (elem.direction == DOWN) {
            newY += elem.length;
            newY--;
        }
        std::cout << newX << ", " << newY << ")" << std::endl;
    }
}
