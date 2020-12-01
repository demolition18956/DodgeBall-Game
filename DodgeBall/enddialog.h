#ifndef ENDDIALOG_H
#define ENDDIALOG_H

#include <QDialog>

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
};

#endif // ENDDIALOG_H
