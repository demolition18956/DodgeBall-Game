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
    connect(hostDialog, SIGNAL(hostNew(int)), this, SLOT(hostNew(int)));
    connect(joinDialog, SIGNAL(backToMain(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(hostDialog, SIGNAL(backToMain(int)), ui->stackedWidget, SLOT(setCurrentIndex(int)));
    connect(joinDialog, SIGNAL(joinNew(QString, int)), this, SLOT(joinNew(QString, int)));

    int a = 1;
    char n = a + 48;
    //qDebug() << n;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::hostFunc()
{
    //qDebug("Host");
    ui->stackedWidget->setCurrentWidget(hostDialog);
}

void MainWindow::joinNew(QString ipAddress, int portNumber) // connect to the game lobby
{
    gameLobby = new lobby(QHostAddress(ipAddress), portNumber, false);

    connect(gameLobby, SIGNAL(showAgain()), this, SLOT(showBack()));

    if(gameLobby->getConnected() == false)
    {
        this->show();
        delete gameLobby;
    }
    else this->hide();

}

void MainWindow::exitFunc()
{
    //qDebug("Exit");
    quitDialog->exec();
}

void MainWindow::exit()
{
    quitDialog->close();
    this->close();
}

void MainWindow::hostNew(int portNumber)
{

    //qDebug("here");
    gameLobby = new lobby(QHostAddress::LocalHost, portNumber, true);
    //qDebug("there");    
    connect(gameLobby, SIGNAL(showAgain()), this, SLOT(showBack()));

    if(gameLobby->getConnected() == false)
    {
        this->show();
        delete gameLobby;
    }
    else this->hide();

}

void MainWindow::joinFunc()
{
    ui->stackedWidget->setCurrentWidget(joinDialog);
    this->show();
    //qDebug() << "here";
}

void MainWindow::showBack()
{
    ui->stackedWidget->setCurrentIndex(0);
    this->show();
}
