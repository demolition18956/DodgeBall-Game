#ifndef MAPDIALOG_H
#define MAPDIALOG_H

#include <QDialog>

namespace Ui {
class mapDialog;
}

class mapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mapDialog(QWidget *parent = nullptr);
    ~mapDialog();

private:
    Ui::mapDialog *ui;
};

#endif // MAPDIALOG_H
