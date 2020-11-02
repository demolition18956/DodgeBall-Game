#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->mainhostbutton, SIGNAL(pressed()), this, SLOT(hostFunc()));
    connect(ui->mainjoinbutton, SIGNAL(pressed()), this, SLOT(joinFunc()));
    connect(ui->mainexitbutton, SIGNAL(pressed()), this, SLOT(exitFunc()));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hostFunc(){
    qDebug("Host");
}

void MainWindow::joinFunc(){
    qDebug("Join");
}

void MainWindow::exitFunc(){
    qDebug("Exit");
}
