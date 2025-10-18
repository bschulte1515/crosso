#ifndef GRID_H
#define GRID_H

#include <vector>
#include <QWidget>
#include "cell.h"
#include "editorstate.h"

class Grid : public QWidget
{
    Q_OBJECT
public:
    explicit Grid(QWidget *parent = nullptr, EditorState *stateIn = nullptr, int g = 15, int c = 30);
    explicit Grid(QWidget *parent = nullptr, EditorState *stateIn = nullptr, QString grid = "", int g = 15, int c = 30);

    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    void toggleCell(int x, int y);
    void switchEditingMode();
    void handleShortcut(QKeyEvent *event);
    void moveSelectedCell();
    QString gridToString();
    void stringToGrid(QString newGrid, int newGridSize);
    void saveToFile();
    void loadFromFile();
    int getGridSize() const { return gridSize; }
    int getCellSize() const { return cellSize; }
    void setGridSize(int size) { gridSize = size; }

    void destroyGrid();

signals:
    void gridResized();

private:
    EditorState *state;

    // These two will later be chosen by the user
    int gridSize;
    int cellSize;

    bool isEditingGrid = false;
    Cell* selectedCell; // Cell currently highlighted when not in grid edit mode
    bool moveDirection = true; // Direction in which to move the selected cell after input (true == vertical, false == horizontal)

    std::vector<std::vector<Cell *>> cells;

    static constexpr int PEN_WIDTH = 1;
};

#endif // GRID_H
