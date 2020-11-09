#include "lobby.h"
#include "ui_lobby.h"



lobby::lobby(QHostAddress ipAddress, int portNumber, bool host_,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::lobby)
{
    ui->setupUi(this);
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    qDebug() << QNetworkInterface::interfaceFromIndex(1).name();
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             qDebug() << address.toString();
    }
    portNum = portNumber;
    address = QHostAddress(ipAddress);
    host = host_;
    if(host ==  true)   // if player is the host
    {
        server = new GameServer();
        socket.connectToHost(ipAddress,portNumber);   // open tcp socket on local machine (where server should be running)
    }
    else
    {
       socket.connectToHost(ipAddress,portNumber);   // non-host connection
    }
    connect(&socket, SIGNAL(connected()), this, SLOT(initialConnect()));
}

lobby::~lobby()
{
    delete ui;
}

void lobby::processMessage(){
    QByteArray datagram;

    QString msg;
    QTextStream in(&datagram, QIODevice::ReadOnly);

    msg = in.readAll();
    qDebug() << msg << "HOST: A player has joined";
}

bool lobby::isHost()
{
    return host;
}

void lobby::initialConnect()
{
    if(!socket.waitForReadyRead(5000))
    {
        qDebug() << "HOST: never got a message!";
    }
    else
    {
        QTextStream incoming(&socket);
        QString msg;
        incoming >> msg;
        qDebug() << socket.bytesAvailable();
        qDebug() << msg;
        int data = msg.toInt();
        if(data == 1)
        {
            qDebug() << "HOST: connection accepted!";
        }
        else if(data == 0)
        {
            qDebug() << "HOST: connection refused!";
            socket.abort();
        }
        else
        {
            qDebug() << "HOST: something went wrong!";
        }
    }
}
