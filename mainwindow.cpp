#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "builder.h"

const int GRID_SIZE = 16 ;
const int CELL_SIZE = 30;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setFixedSize(GRID_SIZE * CELL_SIZE + 40, GRID_SIZE * CELL_SIZE + 40);

    Builder *grid = new Builder(this, GRID_SIZE, CELL_SIZE);
    grid->move(20, 20);
    grid->show();
}

MainWindow::~MainWindow()
{
    delete ui;
}
