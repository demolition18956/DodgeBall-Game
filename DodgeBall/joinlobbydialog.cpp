#include "joinlobbydialog.h"
#include "ui_joinlobbydialog.h"
#include <QRegularExpressionValidator>
#include <QUdpSocket>

JoinLobbyDialog::JoinLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinLobbyDialog)
{
    ui->setupUi(this);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    QRegularExpression ipInput ("[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}");
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(ipInput, ui->ipEdit);
    ui->ipEdit->setValidator(ipValidator);

    QRegularExpression portInput ("[0-9]{1,5}");
    QRegularExpressionValidator* portValidator = new QRegularExpressionValidator(portInput, ui->portEdit);
    ui->portEdit->setValidator(portValidator);

    connect(ui->joinButton, SIGNAL(clicked()), this, SLOT(join()));
}

JoinLobbyDialog::~JoinLobbyDialog()
{
    delete ui;
}

void JoinLobbyDialog::cancel()
{
    this->close();
    setBool(true);
}

bool JoinLobbyDialog::getBool(){
    return back;
}

void JoinLobbyDialog::setBool(bool value){
    back = value;
}

void JoinLobbyDialog::join()
{
    QString ipAddress;
    QString portNumber;

    ipAddress = ui->ipEdit->text();
    portNumber = ui->portEdit->text();
    int portNumber_int = portNumber.toInt();

    socket = new QUdpSocket;
    socket->bind(QHostAddress::AnyIPv4, portNumber_int, QUdpSocket::ReuseAddressHint|QUdpSocket::ShareAddress);
    socket->joinMulticastGroup(QHostAddress(ipAddress));
    QString msg = "Hello";
    QByteArray datagram;
    QTextStream out(&datagram, QIODevice::WriteOnly);
    out << msg << endl;
    socket->writeDatagram(datagram, QHostAddress(ipAddress),portNumber_int);

}

