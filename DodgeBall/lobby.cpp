#include "lobby.h"
#include "ui_lobby.h"



lobby::lobby(bool host_,QWidget *parent) :
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
    host = host_;
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
    qDebug() << msg << "A player has joined";
}

bool lobby::isHost()
{
    return host;
}

