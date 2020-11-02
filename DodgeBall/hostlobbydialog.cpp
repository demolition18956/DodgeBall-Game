#include "hostlobbydialog.h"
#include "ui_hostlobbydialog.h"

HostLobbyDialog::HostLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HostLobbyDialog)
{
    ui->setupUi(this);
}

HostLobbyDialog::~HostLobbyDialog()
{
    delete ui;
}
