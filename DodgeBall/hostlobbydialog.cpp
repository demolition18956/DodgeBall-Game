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
