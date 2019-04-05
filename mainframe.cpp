#include "mainframe.h"
#include "ui_mainframe.h"

MainFrame::MainFrame(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainFrame)
{
    ui->setupUi(this);
}

MainFrame::~MainFrame()
{
    delete ui;
}

void MainFrame::on_actionExit_triggered()
{
    QApplication::exit();
}

void MainFrame::on_btnSimulate_clicked()
{
    qDebug() <<"Simulate (btn)!\n";
}

void MainFrame::on_actionSimulate_triggered()
{
    qDebug() <<"Simulate (action)!\n";
}
