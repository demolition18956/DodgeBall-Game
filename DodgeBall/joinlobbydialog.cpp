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
    QString ipAdress;
    QString portNumber;

    ipAdress = ui->ipEdit->text();
    portNumber = ui->portEdit->text();

    socket = new QUdpSocket;


}
