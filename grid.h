#ifndef GRID_H
#define GRID_H

#include <vector>
#include <QWidget>
#include "cell.h"
#include "lettercell.h"
#include "direction.h"

class State;

struct Word {
    int startX;
    int startY;
    int length;
    Direction direction;
    // TODO
    // int clueNumber;
    // QString clue;
};

class Grid : public QWidget
{
    Q_OBJECT
public:
    explicit Grid(QWidget *parent = nullptr, State *stateIn = nullptr, int g = 15, int c = 30);

    void resetGrid();
    void destroyGrid();

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    LetterCell *getFirstLetter();
    LetterCell *getNextLetter(LetterCell *cell, Direction direction);
    void addHighlighting();
    void removeHighlighting(LetterCell *cell);
    void updateHighlighting(LetterCell *oldCell);
    void updateSelectedCell(int x, int y);
    void toggleCells(int x, int y, bool symmetric);
    void switchEditingMode();
    void handleShortcut(QKeyEvent *event);
    QString toString();
    void fromString(QString newGrid, int newGridSize);
    void saveToFile();
    void loadFromFile();
    int getSize() const { return size; }
    int getCellSize() const { return cellSize; }
    void setSize(int newSize) { size = newSize; }
    auto& getCells() { return cells; }

    bool startsWord(LetterCell *cell, Direction direction);
    struct Word parseWord(LetterCell *cell, Direction direction);
    int findWord(int x, int y, Direction direction);
    std::vector<LetterCell *> wordToCells(struct Word &word);
    void updateWords();
    void printWord(struct Word &word);
    void printWords();

signals:
    void gridResized();

private:
    State *state;

    // These two will later be chosen by the user
    int size;
    int cellSize;
    std::vector<std::vector<Cell *>> cells;
    std::vector<struct Word> words;

    static constexpr int PEN_WIDTH = 1;
};

#endif // GRID_H
