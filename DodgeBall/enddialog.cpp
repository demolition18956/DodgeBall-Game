#include "enddialog.h"
#include "ui_enddialog.h"

endDialog::endDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::endDialog)
{
    ui->setupUi(this);
}

endDialog::~endDialog()
{
    delete ui;
}

void endDialog::exitFunc()
{
    //qDebug("Exit");
    quitDialog->exec();
}

void endDialog::exit()
{
    quitDialog->close();
    this->close();
}

void endDialog::determineWinner(int winner){
    if(winner == 1){
        ui->winningTeamLabel->setText("Blue");
    }
    else
         ui->winningTeamLabel->setText("Red");
}
