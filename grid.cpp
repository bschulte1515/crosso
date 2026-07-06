#include <QPainter>
#include <QMouseEvent>
#include <QKeyEvent>
#include <QFileDialog>
#include <QMessageBox>
#include <iostream>

#include "grid.h"
#include "state.h"
#include "lettercell.h"
#include "blackcell.h"
#include "word.h"

Grid::Grid(QWidget *parent, State *stateIn, int g, int c)
    : QWidget{parent}, state(stateIn), size(g), cellSize(c)
{
    // For each direction (horizontally and vertically),
    // 	The grid will have @p size + 1 "inner lines" of width INNER_LINE_WIDTH
    //  The grid will have 2 "border lines" of width BORDER_LINE_WIDTH
    // 	The grid will have @p size cells of width cellSize
    this->setFixedSize(
        ((size + 1) * inner_line_width) + (2 * border_line_width) + (size * cellSize),
        ((size + 1) * inner_line_width) + (2 * border_line_width) + (size * cellSize));

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
    state->selectCell(firstLetter);
}

/**
 * @brief Draw each word (i.e. clue numbers) and highlight selected word/cell
 * @param painter
 */
void Grid::drawWords(QPainter *painter)
{
    LetterCell *selected = state->getSelectedCell();
    painter->fillRect(selected->toRect(), SELECTED_COLOR);
    selected->drawLetter(painter, Qt::white); // Replace highlighted letters with white

    int word_index = findWord(selected->getX(), selected->getY(), state->getFillDirection());
    if (word_index != -1) {
        for (LetterCell *letter : wordToCells(words[word_index])) {
            if (letter == selected) continue;
            painter->fillRect(letter->toRect(), HIGHLIGHT_COLOR);
            letter->drawLetter(painter, Qt::white); // Replace highlighted letters with white
        }
    }

    // Give a light gray, transparent background to word in other direction
    word_index = findWord(selected->getX(), selected->getY(), (Direction)!state->getFillDirection());
    if (word_index != -1) {
        for (LetterCell *letter : wordToCells(words[word_index])) {
            if (letter == selected) continue;
            painter->fillRect(letter->toRect(), PERPENDICULAR_COLOR);
            letter->drawLetter(painter, Qt::black); // Write over letters again
        }
    }

    // Draw each clue number
    QFont font = painter->font();
    font.setPointSize(this->clue_number_font_size);
    painter->setFont(font);
    painter->setPen(Qt::black);
    for (auto &word : words) {
        painter->drawText(
            cells[word.startX][word.startY]->toRectWithOffset(2, 1),
            Qt::AlignTop | Qt::AlignLeft,
            QString::number(word.clueNumber)
        );
    }
}

void Grid::drawBorder(QPainter *painter)
{
    QPen pen(Qt::black);
    pen.setWidth(border_line_width);
    pen.setCapStyle(Qt::SquareCap);
    painter->setPen(pen);
    painter->drawRect(0, 0, width() - 1, height() - 1);
    painter->drawRect(1, 1, width() - 1, height() - 1);
}

void Grid::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Paint background white and draw black border
    painter.fillRect(rect(), Qt::white);
    drawBorder(&painter);

    painter.translate(border_line_width, border_line_width);

    // Draw each of the cells
    for (int i = 0; i < size; ++i) {
        for (int j = 0; j < size; ++j) {
            cells[i][j]->draw(&painter);
        }
    }

    // Draw each of the clue numbers/highlight
    if (state->getEditingMode() == FILL) {
        drawWords(&painter);
    }

    // Draw the lines between cells
    QPen pen(Qt::darkGray);
    pen.setWidth(inner_line_width);
    pen.setCapStyle(Qt::SquareCap);
    painter.setPen(pen);

    for (int row = 0; row <= size; row++) {
        painter.drawLine(
            0,
            row * (cellSize + inner_line_width),
            size * (cellSize + inner_line_width),
            row * (cellSize + inner_line_width)
        );
    }
    for (int col = 0; col <= size; col++) {
        painter.drawLine(
            col * (cellSize + inner_line_width),
            0,
            col * (cellSize + inner_line_width),
            size * (cellSize + inner_line_width)
        );
    }
}

void Grid::toggleCell(Cell *cell, bool symmetric)
{
    bool wasBlack = cell->isBlack();
    int x = cell->getX();
    int y = cell->getY();

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
        break;
    }
    case FILL: {
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
    if (event->position().x() <= border_line_width || event->position().y() <= border_line_width) return;
    int x = (event->position().x() - border_line_width) / (cellSize + inner_line_width);
    int y = (event->position().y() - border_line_width) / (cellSize + inner_line_width);
    if (x >= size || y >= size) return;

    Cell *cell = cells[x][y];

    if (state->getEditingMode() == LAYOUT) {
        bool symmetricGrid = true;
        toggleCell(cell, symmetricGrid);
    } else {
        state->selectCell(cell);
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
            state->getSelectedCell()->setLetter(ch);
            state->selectCell(
                getNextLetter(state->getSelectedCell(), state->getFillDirection())
            );
        }
    }

exit:
    update();
}

LetterCell *Grid::getFirstLetter()
{
    if (words.empty()) return NULL;
    struct Word &firstWord = words[0];
    Cell *firstCell = cells[firstWord.startX][firstWord.startY];
    return dynamic_cast<LetterCell *>(firstCell);
}

/**
 * @brief Returns a pointer to the "next" cell. The next cell will either be the cell directly
 * 		  right or down, based on direction
 * @param cell					 The cell to start at
 * @param direction				 The direction in which to traverse the cells
 * @return 						 The next cell. Set to NULL if traversing would lead to OOB
 */
LetterCell *Grid::getNextLetter(LetterCell *cell, Direction direction)
{
    int oldX = cell->getX();
    int oldY = cell->getY();
    int deltaX = (direction == ACROSS &&
                  !(oldX == size - 1)) ? 1 : 0;
    int deltaY = (direction == DOWN &&
                  !(oldY == size - 1)) ? 1 : 0;
    if (deltaX == 0 && deltaY == 0) return NULL;
    return dynamic_cast<LetterCell *>(cells[oldX + deltaX][oldY + deltaY]);
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

    this->setFixedSize(
        ((size + 1) * inner_line_width) + (2 * border_line_width) + (size * cellSize),
        ((size + 1) * inner_line_width) + (2 * border_line_width) + (size * cellSize));

    emit gridResized();
    resetGrid();
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
bool Grid::startsWord(LetterCell *cell, Direction direction)
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
 * @param number
 * @return
 */
struct Word Grid::parseWord(LetterCell *cell, Direction direction, int number)
{
    struct Word newWord = {
        .startX = cell->getX(),
        .startY = cell->getY(),
        .length = 0,
        .direction = direction,
        .clueNumber = number
    };
    for (LetterCell *next = cell;
         next != NULL;
         next = getNextLetter(next, direction))
    {
        newWord.length++;
    }
    return newWord;
}

int Grid::findWord(int x, int y, Direction direction)
{
    for (unsigned int i = 0; i < words.size(); i++) {
        auto &word = words[i];
        if (direction == ACROSS && word.direction == ACROSS &&
           word.startX <= x &&
           x <= (word.startX + word.length - 1) &&
           word.startY == y) {
            return i;
        }
        else if (direction == DOWN && word.direction == DOWN &&
                word.startY <= y &&
                y <= (word.startY + word.length - 1) &&
                word.startX == x) {
            return i;
        }
    }
    return -1;
}

std::vector<LetterCell *> Grid::wordToCells(struct Word &word)
{
    std::vector<LetterCell *> wordAsVector;
    LetterCell *cell =
        dynamic_cast<LetterCell *>(cells[word.startX][word.startY]);
    while (cell) {
        wordAsVector.push_back(cell);
        cell = getNextLetter(cell, word.direction);
    }
    if ((int)wordAsVector.size() != word.length) {
        std::cout << "Something went wrong converting word to cells!" << std::endl;
        return {};
    }
    return wordAsVector;
}

/**
 * @brief Grid::updateWords      Parses the grid into words, first across then down
 *
 * Called everytime there is an update to the grid. For right now this should only be when switching
 * from LAYOUT -> FILL mode, or after initalizing the grid (regularly or fromString)
 */
void Grid::updateWords()
{
    int clueNumber = 1;
    bool wordParsed = false;
    words.clear();
    Cell *cell = nullptr;
    LetterCell *letter = nullptr;
    for (int y = 0; y < size; y++) {
        for (int x = 0; x < size; x++) {
            cell = cells[x][y];
            if (cell->isBlack()) continue;
            letter = dynamic_cast<LetterCell *>(cell);
            wordParsed = false;
            if (letter && startsWord(letter, ACROSS)) {
                words.push_back(parseWord(letter, ACROSS, clueNumber));
                wordParsed = true;
            }
            if (letter && startsWord(letter, DOWN)) {
                words.push_back(parseWord(letter, DOWN, clueNumber));
                wordParsed = true;
            }
            if (wordParsed) clueNumber++;
        }
    }
}

void Grid::printWord(struct Word &word)
{
    std::cout << "(" << word.startX << ", " << word.startY << ") -> (";
    int newX = word.startX;
    int newY = word.startY;
    if (word.direction == ACROSS) {
        newX += word.length;
        newX--;
    }
    if (word.direction == DOWN) {
        newY += word.length;
        newY--;
    }
    std::cout << newX << ", " << newY << ")" << std::endl;
}

void Grid::printWords()
{
    std::cout << "Words:" << std::endl;
    for (auto &word : words) {
        printWord(word);
    }
}
