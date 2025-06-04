#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::KaijuFight)
{
    ui->setupUi(this);
    char what = *ui->centralwidget->whatsThis().toStdString().c_str();
}

MainWindow::~MainWindow()
{
    delete ui;
}
