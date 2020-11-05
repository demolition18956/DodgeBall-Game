#include "lobby.h"
#include "ui_lobby.h"



lobby::lobby(QWidget *parent) :
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
    hostUDP = new QUdpSocket(this);
    hostUDP->bind(5678);
    hostUDP->setMulticastInterface(QNetworkInterface::interfaceFromName("p6p2"));

    connect(hostUDP, SIGNAL(readyRead()), this, SLOT(processMessage()));
}

lobby::~lobby()
{
    delete ui;
}

void lobby::processMessage(){
    QByteArray datagram;

        while (hostUDP->hasPendingDatagrams())
        {
            datagram.resize(hostUDP->pendingDatagramSize());
            hostUDP->readDatagram(datagram.data(), datagram.size());
        }

        /*QString msg;
        QTextStream in(&datagram, QIODevice::ReadOnly);

        msg = in.readLine();*/
        qDebug() << "A payer has joined";
}
