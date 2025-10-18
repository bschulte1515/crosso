#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "grid.h"
#include "editorstate.h"

const int GRID_SIZE = 15;
const int CELL_SIZE = 30;
const int BORDER_PX = 20;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , state(new EditorState())
{
    ui->setupUi(this);

    setFixedSize(GRID_SIZE * CELL_SIZE + BORDER_PX * 2, GRID_SIZE * CELL_SIZE + BORDER_PX * 2);

    state = new EditorState();
    currentGrid = new Grid(this, state, GRID_SIZE, CELL_SIZE);
    currentGrid->move(BORDER_PX, BORDER_PX);
    currentGrid->show();

    connect(currentGrid, SIGNAL(gridResized()), this, SLOT(on_gridResized()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_gridResized()
{
    setFixedSize(currentGrid->getGridSize() * currentGrid->getCellSize() + BORDER_PX * 2, currentGrid->getGridSize() * currentGrid->getCellSize() + BORDER_PX * 2);
}
