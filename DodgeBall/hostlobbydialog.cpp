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

    // Input validation, ensures that only a maximum of 5 integers can be input for port number
    QRegularExpression portInput ("[0-9]{1,5}");
    QRegularExpressionValidator* portValidator = new QRegularExpressionValidator(portInput, ui->portEdit);
    ui->portEdit->setValidator(portValidator);
    ui->portEdit->setText("2224");
}

HostLobbyDialog::~HostLobbyDialog()
{
    delete ui;
}

void HostLobbyDialog::cancel()
{
    emit backToMain(0);
}

bool HostLobbyDialog::getBool()
{
    return back;
}

void HostLobbyDialog::setBool(bool value)
{
    back = value;
}

void HostLobbyDialog::createLobby()
{
    QString portNum;

    portNum = ui->portEdit->text();
    int portNum_int = portNum.toInt();
    emit hostNew(portNum_int);
}

