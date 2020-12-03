#ifndef SCOREDIALOG_H
#define SCOREDIALOG_H

#include <QDialog>

namespace Ui {
class scoreDialog;
}

class scoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit scoreDialog(QWidget *parent = nullptr);
    ~scoreDialog();

private:
    Ui::scoreDialog *ui;
};

#endif // SCOREDIALOG_H
