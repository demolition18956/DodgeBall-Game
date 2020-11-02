#include "joinlobbydialog.h"
#include "ui_joinlobbydialog.h"

JoinLobbyDialog::JoinLobbyDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::JoinLobbyDialog)
{
    ui->setupUi(this);
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancel()));
}

JoinLobbyDialog::~JoinLobbyDialog()
{
    delete ui;
}

void JoinLobbyDialog::cancel()
{
    this->close();
}
