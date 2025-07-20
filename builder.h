#ifndef BUILDER_H
#define BUILDER_H

#include <QWidget>

class Builder : public QWidget
{
    Q_OBJECT
public:
    explicit Builder(QWidget *parent = nullptr, int g = 15, int c = 30);

    void paintEvent(QPaintEvent *event) override;
    int getGridSize() const { return gridSize; }
    int getCellSize() const { return cellSize; }
    void setGridSize(int size) { gridSize = size; }

private:
    // These two will later be chosen by the user
    int gridSize;
    int cellSize;

    static constexpr int PEN_WIDTH = 1;
};

#endif // BUILDER_H
