#ifndef ENDDIALOG_H
#define ENDDIALOG_H

#include <QDialog>
#include <quitdialog.h>

namespace Ui {
class endDialog;
}

class endDialog : public QDialog
{
    Q_OBJECT

public:
    explicit endDialog(QWidget *parent = nullptr);
    ~endDialog();

private:
    Ui::endDialog *ui;
    QuitDialog *quitDialog;

private slots:
    void exit();

public slots:
    void exitFunc();
    void determineWinner(int winner);
};

#endif // ENDDIALOG_H
