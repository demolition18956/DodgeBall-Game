#ifndef QUITDIALOG_H
#define QUITDIALOG_H

#include <QDialog>

namespace Ui {
class QuitDialog;
}

class QuitDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuitDialog(QWidget *parent = nullptr);
    ~QuitDialog();

private:
    Ui::QuitDialog *ui;

signals:
    void quit();
};

#endif // QUITDIALOG_H
