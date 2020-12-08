#include "joinlobbydialog.h"
#include "ui_joinlobbydialog.h"
#include <QRegularExpressionValidator>

JoinLobbyDialog::JoinLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinLobbyDialog)
{
    ui->setupUi(this);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    // Input validation, ensures that IP address consists only of integers in correct format
    QRegularExpression ipInput ("[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}");
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(ipInput, ui->ipEdit);
    ui->ipEdit->setValidator(ipValidator);

    // Input validation, ensures that only a maximum of 5 integers can be input for port number
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
    emit backToMain(0);
}

bool JoinLobbyDialog::getBool()
{
    return back;
}

void JoinLobbyDialog::setBool(bool value)
{
    back = value;
}

void JoinLobbyDialog::join() // Join lobby based of port and IP inputs
{
    QString ipAddress;
    QString portNumber;

    ipAddress = ui->ipEdit->text();
    portNumber = ui->portEdit->text();
    int portNumber_int = portNumber.toInt();
    emit joinNew(ipAddress, portNumber_int);
}


