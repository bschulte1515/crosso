#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "builder.h"

const int GRID_SIZE = 20;
const int CELL_SIZE = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    Builder *grid = new Builder(this, GRID_SIZE, CELL_SIZE);
    grid->move(20, 20);
    grid->show();

    setFixedSize(GRID_SIZE * CELL_SIZE + 40, GRID_SIZE * CELL_SIZE + 40);
}

MainWindow::~MainWindow()
{
    delete ui;
}
