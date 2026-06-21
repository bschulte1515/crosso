#ifndef GRID_H
#define GRID_H

#include <vector>
#include <QWidget>
#include "cell.h"
#include "lettercell.h"

class State;

class Grid : public QWidget
{
    Q_OBJECT
public:
    explicit Grid(QWidget *parent = nullptr, State *stateIn = nullptr, int g = 15, int c = 30);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void getNextCell(int *x, int *y);
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

    void updateRows();
    void updateCols();

    void destroyGrid();

signals:
    void gridResized();

private:
    State *state;

    // These two will later be chosen by the user
    int size;
    int cellSize;
    std::vector<std::vector<Cell *>> cells;

    // Each row/col is represented as a pair of coordinate points
    // (1, 0), (5, 0) would be a row of length 5, for example
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> rows;
    std::vector<std::pair<std::pair<int, int>, std::pair<int, int>>> cols;

    static constexpr int PEN_WIDTH = 1;
};

#endif // GRID_H
