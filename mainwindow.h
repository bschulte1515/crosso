#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "grid.h"
#include "state.h"
#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_gridResized();

private:
    Ui::MainWindow *ui;
    Grid *grid;
    State *state;
};
#endif // MAINWINDOW_H
