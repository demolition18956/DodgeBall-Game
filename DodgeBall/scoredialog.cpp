#include "scoredialog.h"
#include "ui_scoredialog.h"
#include "mapdialog.h"

scoreDialog::scoreDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::scoreDialog)
{
    ui->setupUi(this);
    _close = false;
    connect(ui->closeButton, SIGNAL(clicked()), this, SLOT(setClose()));

}

scoreDialog::~scoreDialog()
{
    delete ui;
}

void scoreDialog::setData(int throws, int time)  //int index, int throws, int kills <-- original parameters
{
    //qDebug() << "Into the SetData";

    QString text;
    text = QString::number(throws);
    ui->ballsThrown->setText(text);

    text.clear();
    text = QString::number(time);
    text.append(" s");
    ui->timeLabel->setText(text);
}

bool scoreDialog::getClose()
{
    return _close;
}

//set close to true so we can exit the while loop in mapDialog
void scoreDialog::setClose()
{
    _close = true;
    this->close();
}
