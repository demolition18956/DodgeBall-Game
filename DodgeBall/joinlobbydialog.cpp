#include "joinlobbydialog.h"
#include "ui_joinlobbydialog.h"
#include <QRegularExpressionValidator>

JoinLobbyDialog::JoinLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinLobbyDialog)
{
    ui->setupUi(this);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    ui->ipEdit->setText("146.229.162.184");
    QRegularExpression ipInput ("[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}");
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(ipInput, ui->ipEdit);
    ui->ipEdit->setValidator(ipValidator);

    ui->portEdit->setText("5678");
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

}

