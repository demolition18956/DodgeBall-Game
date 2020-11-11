#include "lobby.h"
#include "ui_lobby.h"

lobby::lobby(QHostAddress ipAddress, int portNumber, bool host_,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::lobby)
{
    ui->setupUi(this);
    connect(ui->readyButton, SIGNAL(pressed()), this, SLOT(playerReady()));
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    qDebug() << QNetworkInterface::interfaceFromIndex(1).name();
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             qDebug() << address.toString();
    }
    portNum = portNumber;
    address = QHostAddress(ipAddress);
    host = host_;
    connect(&socket, SIGNAL(connected()), this, SLOT(initialConnect()));
    if(host ==  true)   // if player is the host
    {
        server = new GameServer();
        socket.connectToHost(QHostAddress::LocalHost, portNum);   // open tcp socket on local machine (where server should be running)

    }
    else
    {
       socket.connectToHost(ipAddress,portNumber);   // non-host connection
       if(!socket.waitForConnected(5000))
       {
           qDebug() << "CLIENT: Couldn't Connect due to errors";
       }
    }
    connect(&socket, SIGNAL(readyRead()),this, SLOT(processMessage()));

}

lobby::~lobby()
{
    delete ui;
}

void lobby::processMessage(){
    QString msg;
    QTextStream in(&socket);
    msg = in.readAll();
    qDebug() << msg << "CLIENT: A player has joined";
}

bool lobby::isHost()
{
    return host;
}

void lobby::initialConnect()
{
    connect(&socket, SIGNAL(readyRead()),this, SLOT(processMessage()));


}

void lobby::playerReady()
{
    static bool readyPrev = false;
    qDebug("Player ready");
    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    if (!readyPrev){
        out << QString::number(1) << " Ready" << endl;
        ui->readyButton->setText("UnReady");
    }
    else {
        out << QString::number(1) << " NotReady" << endl;
        ui->readyButton->setText("Ready");
    }
    readyPrev ^= true;
    socket.write(block);
    qDebug() << block;
}
