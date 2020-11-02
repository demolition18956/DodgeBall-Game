#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    quitDialog = new QuitDialog(this);
    hostDialog = new HostLobbyDialog(this);
    joinDialog = new JoinLobbyDialog(this);
    connect(ui->mainhostbutton, SIGNAL(pressed()), this, SLOT(hostFunc()));
    connect(ui->mainjoinbutton, SIGNAL(pressed()), this, SLOT(joinFunc()));
    connect(ui->mainexitbutton, SIGNAL(pressed()), this, SLOT(exitFunc()));
    connect(quitDialog, SIGNAL(quit()), this, SLOT(exit()));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete quitDialog;
    delete hostDialog;
    delete joinDialog;
}

void MainWindow::hostFunc(){
    qDebug("Host");
    hostDialog->exec();
}

void MainWindow::joinFunc(){
    qDebug("Join");
    joinDialog->exec();
}

void MainWindow::exitFunc(){
    qDebug("Exit");
    quitDialog->exec();
}

void MainWindow::exit()
{
    quitDialog->close();
    this->close();
}
