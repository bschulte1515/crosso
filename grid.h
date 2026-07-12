#ifndef GRID_H
#define GRID_H

#include <vector>
#include <QWidget>
#include "cell.h"
#include "blackcell.h"
#include "lettercell.h"
#include "direction.h"
#include "word.h"

static constexpr QColor HIGHLIGHT_COLOR(10, 30, 255, 140);
static constexpr QColor SELECTED_COLOR(10, 30, 255, 200);
static constexpr QColor PERPENDICULAR_COLOR(220, 221, 220, 130);

class State;

class Grid : public QWidget
{
    Q_OBJECT
public:
    explicit Grid(QWidget *parent = nullptr, State *stateIn = nullptr, int g = 15, int c = 30);
    void destroyGrid();

    void drawWords(QPainter *painter);
    void drawBorder(QPainter *painter);
    void paintEvent(QPaintEvent *event) override;

    void enterLetter(QChar ch);
    void removeLetter();
    void keyPressEvent(QKeyEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

    Cell *getCell(int x, int y);
    LetterCell *getLetterCell(int x, int y);
    BlackCell *getBlackCell(int x, int y);
    Cell *getAdjacentCell(Cell *cell, WordDirection direction);
    Cell *getAdjacentCell(Cell *cell, MoveDirection direction);
    LetterCell *getAdjacentLetterCell(Cell *cell, MoveDirection direction);
    BlackCell *getAdjacentBlackCell(Cell *cell, MoveDirection direction);
    LetterCell *getFirstLetter();
    LetterCell *getNextLetter(LetterCell *cell, WordDirection direction);

    bool startsWord(LetterCell *cell, WordDirection direction);
    struct Word parseWord(LetterCell *cell, WordDirection direction, int number);
    int findWord(int x, int y, WordDirection direction);
    std::vector<LetterCell *> wordToCells(struct Word &word);
    void refreshWords();
    void printWord(struct Word &word);
    void printWords();

    void toggleCell(Cell *cell, bool symmetric);
    void switchActiveMode();
    QString toString();
    void fromString(QString newGrid, int newGridSize);
    void saveToFile();
    void loadFromFile();

    /* Getters */
    auto& getCells() { return cells; }
    int getSize() const { return size; }
    int getCellSize() const { return cellSize; }

    /* Setters */
    void setSize(int newSize) { size = newSize; }

    /* Constants for drawing the grid */
    static constexpr int inner_line_width = 1;
    static constexpr int border_line_width = 2;
    static constexpr int letter_font_size = 14;
    static constexpr int clue_number_font_size = 7;
    static constexpr int letter_offset_y = 5;

signals:
    void gridResized();

private:
    State *state;

    // These two will later be chosen by the user
    int size;
    int cellSize;
    std::vector<std::vector<Cell *>> cells;
    std::vector<struct Word> words;
};

#endif // GRID_H
