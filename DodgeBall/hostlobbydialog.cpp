#include "hostlobbydialog.h"
#include "ui_hostlobbydialog.h"
#include <QRegularExpressionValidator>

HostLobbyDialog::HostLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostLobbyDialog)
{
    ui->setupUi(this);
    back = false;
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));

    QRegularExpression ipInput ("[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}[.]{1}[0-9]{1,3}");
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(ipInput, ui->ipEdit);
    ui->ipEdit->setValidator(ipValidator);

    QRegularExpression portInput ("[0-9]{1,5}");
    QRegularExpressionValidator* portValidator = new QRegularExpressionValidator(portInput, ui->portEdit);
    ui->portEdit->setValidator(portValidator);
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
