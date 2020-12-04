#ifndef SCOREDIALOG_H
#define SCOREDIALOG_H

#include <QDialog>
#include <QTimer>
#include "player.h"

namespace Ui {
class scoreDialog;
}

class scoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit scoreDialog(QWidget *parent = nullptr);
    void setData(int throws, int time);
    bool getClose();
    ~scoreDialog();

private:
    Ui::scoreDialog *ui;
    bool _close;

private slots:
    void setClose();
};

#endif // SCOREDIALOG_H
