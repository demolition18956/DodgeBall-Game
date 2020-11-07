#include "hostlobbydialog.h"
#include "ui_hostlobbydialog.h"
#include "lobby.h"
#include <QRegularExpressionValidator>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QtDebug>

HostLobbyDialog::HostLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostLobbyDialog)
{
    ui->setupUi(this);
    back = false;
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
    connect(ui->joinButton, SIGNAL(clicked()), this, SLOT(createLobby()));

    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    qDebug() << QNetworkInterface::interfaceFromIndex(1).name();
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             qDebug() << address.toString();
    }

}

HostLobbyDialog::~HostLobbyDialog()
{
    delete ui;
}

void HostLobbyDialog::cancel()
{
    this->close();
    setBool(true);
}

bool HostLobbyDialog::getBool(){
    return back;
}

void HostLobbyDialog::setBool(bool value){
    back = value;
}

void HostLobbyDialog::createLobby(){
    lobby *gameLobby = new lobby(true);
    GameServer *server = new GameServer;
    this->hide();
    gameLobby->show();
}

void HostLobbyDialog::processMessage(){
    QByteArray datagram;

        while (hostUDP->hasPendingDatagrams())
        {
            datagram.resize(hostUDP->pendingDatagramSize());
            hostUDP->readDatagram(datagram.data(), datagram.size());
        }

        QString msg;
        QTextStream in(&datagram, QIODevice::ReadOnly);

        msg = in.readLine();
        qDebug() << msg;
}
