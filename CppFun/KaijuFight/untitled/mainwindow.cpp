#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::KaijuFight)
{
    ui->setupUi(this);
}

void MainWindow::setCommandBoxText(std::string s)
{
    ui->CommandBox->setText(s.c_str());
}

MainWindow::~MainWindow()
{
    delete ui;
}
