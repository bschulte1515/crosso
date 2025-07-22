#ifndef BUILDER_H
#define BUILDER_H

#include <QWidget>
#include <vector>
#include "cell.h"

class Builder : public QWidget
{
    Q_OBJECT
public:
    explicit Builder(QWidget *parent = nullptr, int g = 15, int c = 30);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void toggleCell(int x, int y);
    int getGridSize() const { return gridSize; }
    int getCellSize() const { return cellSize; }
    void setGridSize(int size) { gridSize = size; }

private:
    // These two will later be chosen by the user
    int gridSize;
    int cellSize;
    bool isEditingGrid = false;
    Cell* selectedCell; // Cell currently highlighted when not in grid edit mode

    std::vector<std::vector<Cell *>> grid;

    static constexpr int PEN_WIDTH = 1;
};

#endif // BUILDER_H
