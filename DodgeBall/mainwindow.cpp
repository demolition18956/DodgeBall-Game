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
    ui->stackedWidget->addWidget(hostDialog);
    ui->stackedWidget->addWidget(joinDialog);
    connect(ui->mainhostbutton, SIGNAL(pressed()), this, SLOT(hostFunc()));
    connect(ui->mainjoinbutton, SIGNAL(pressed()), this, SLOT(joinFunc()));
    connect(ui->mainexitbutton, SIGNAL(pressed()), this, SLOT(exitFunc()));
    connect(quitDialog, SIGNAL(quit()), this, SLOT(exit()));
    connect(hostDialog, SIGNAL(hostNew()), this, SLOT(hostNew()));
    connect(joinDialog, SIGNAL(backToMain(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(hostDialog, SIGNAL(backToMain(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
}

MainWindow::~MainWindow()
{
    delete ui;
    delete quitDialog;
    delete hostDialog;
    delete joinDialog;
    delete gameLobby;
    delete server;
}

void MainWindow::hostFunc(){
    qDebug("Host");
//    this->hide();
//    hostDialog->exec();

//    if(hostDialog->getBool()){
//        this->show();
//    }
    ui->stackedWidget->setCurrentWidget(hostDialog);
}

void MainWindow::joinFunc(){
//    qDebug("Join");
//    this->hide();
//    joinDialog->exec();

//    if(joinDialog->getBool()){
//        this->show();
//    }
    ui->stackedWidget->setCurrentWidget(joinDialog);

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

void MainWindow::hostNew()
{
    gameLobby = new lobby(true);
    server = new GameServer();
    gameLobby->show();
}
