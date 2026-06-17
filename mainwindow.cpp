#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "grid.h"
#include "state.h"

const int GRID_SIZE = 15;
const int CELL_SIZE = 30;
const int BORDER_PX = 20;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , state(new State())
{
    ui->setupUi(this);

    setFixedSize(GRID_SIZE * CELL_SIZE + BORDER_PX * 2, GRID_SIZE * CELL_SIZE + BORDER_PX * 2);

    grid = new Grid(this, state, GRID_SIZE, CELL_SIZE);
    state->setGrid(grid);
    grid->move(BORDER_PX, BORDER_PX);
    grid->show();

    connect(grid, SIGNAL(gridResized()), this, SLOT(on_gridResized()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_gridResized()
{
    setFixedSize(grid->getSize() * grid->getCellSize() + BORDER_PX * 2, grid->getSize() * grid->getCellSize() + BORDER_PX * 2);
}
