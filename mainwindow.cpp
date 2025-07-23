#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "grid.h"
#include "editorstate.h"

const int GRID_SIZE = 16;
const int CELL_SIZE = 30;
const int BORDER_PX    = 20;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(GRID_SIZE * CELL_SIZE + BORDER_PX * 2, GRID_SIZE * CELL_SIZE + BORDER_PX * 2);

    Grid *grid = new Grid(this, new EditorState(), GRID_SIZE, CELL_SIZE);
    grid->move(BORDER_PX, BORDER_PX);
    grid->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
