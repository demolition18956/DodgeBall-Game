#include "joinlobbydialog.h"
#include "ui_joinlobbydialog.h"

JoinLobbyDialog::JoinLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinLobbyDialog)
{
    ui->setupUi(this);
}

JoinLobbyDialog::~JoinLobbyDialog()
{
    delete ui;
}
