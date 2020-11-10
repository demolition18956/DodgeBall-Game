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
        socket.connectToHost(ipAddress,portNumber);   // open tcp socket on local machine (where server should be running)
    }
    else
    {
       socket.connectToHost(ipAddress,portNumber);   // non-host connection
       if(!socket.waitForConnected(5000))
       {
           qDebug() << "CLIENT: Couldn't Connect";
       }
    }

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
    qDebug() << msg << "CLIENT: A player has joined";
}

bool lobby::isHost()
{
    return host;
}

void lobby::initialConnect()
{
    if(socket.waitForReadyRead(5000))
    {
        QTextStream incoming(&socket);
        QString msg;
        incoming >> msg;
        int data = msg.toInt();
        if(data == 1)
        {
            qDebug() << "CLIENT: connection accepted!";
        }
        else if(data == 0)
        {
            qDebug() << "CLIENT: connection refused!";
            socket.abort();
        }
        else if(data == 2)
        {
            //QString s = ui->playercountLabel
            int n = ui->playercountLabel->text().toInt();
            n++;
            ui->playercountLabel->setText(QString::number(n));
        }
        else
        {
            qDebug() << "CLIENT: something went wrong!";
        }
    }
    else
    {
        qDebug() << "CLIENT: Never got a message!";
    }

}

void lobby::playerReady()
{
    emit ready();
    qDebug("Player ready");
}
