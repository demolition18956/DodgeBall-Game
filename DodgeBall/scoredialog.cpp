#include "scoredialog.h"
#include "ui_scoredialog.h"
#include "mapdialog.h"

scoreDialog::scoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::scoreDialog)
{
    ui->setupUi(this);
    close = false;
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(setClose()));

}

scoreDialog::~scoreDialog()
{
    delete ui;
}

void scoreDialog::setData(int index, int throws, int kills)
{
    double accuracy = kills/throws;
    switch (index)
    {

    case 0 :
        ui->p1Accuracy->setText(QString::number(accuracy));
        ui->p1Thrown->setText(QString::number(throws));
        ui->p1Elim->setText(QString::number(kills));
    break;

    case 1 :
        ui->p2Accuracy->setText(QString::number(accuracy));
        ui->p2Thrown->setText(QString::number(throws));
        ui->p2Elim->setText(QString::number(kills));
    break;

    case 2 :
        ui->p3Accuracy->setText(QString::number(accuracy));
        ui->p3Thrown->setText(QString::number(throws));
        ui->p3Elim->setText(QString::number(kills));
    break;

    case 3 :
        ui->p4Accuracy->setText(QString::number(accuracy));
        ui->p4Thrown->setText(QString::number(throws));
        ui->p4Elim->setText(QString::number(kills));
    break;

    case 4 :
        ui->p5Accuracy->setText(QString::number(accuracy));
        ui->p5Thrown->setText(QString::number(throws));
        ui->p5Elim->setText(QString::number(kills));
    break;

    case 5 :
        ui->p6Accuracy->setText(QString::number(accuracy));
        ui->p6Thrown->setText(QString::number(throws));
        ui->p6Elim->setText(QString::number(kills));
    break;

    }
}

bool scoreDialog::getClose()
{
    return close;
}

//set close to true so we can exit the while loop in mapDialog
void scoreDialog::setClose()
{
    close = true;
}
