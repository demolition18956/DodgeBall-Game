#include "quitdialog.h"
#include "ui_quitdialog.h"
#include <QtDebug>

QuitDialog::QuitDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::QuitDialog)
{
    ui->setupUi(this);
    connect(ui->yesButton, SIGNAL(clicked()), this, SLOT(yes()));
    connect(ui->noButton, SIGNAL(clicked()), this, SLOT(no()));
}

QuitDialog::~QuitDialog()
{
    delete ui;
}

void QuitDialog::yes()
{
    emit quit();
}

void QuitDialog::no()
{
    this->close();
}
