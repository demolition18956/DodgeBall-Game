#ifndef SCOREDIALOG_H
#define SCOREDIALOG_H

#include <QDialog>
#include "player.h"

namespace Ui {
class scoreDialog;
}

class scoreDialog : public QDialog
{
    Q_OBJECT

public:
    explicit scoreDialog(QWidget *parent = nullptr);
    void setData(int index, int throws, int kills);
    bool getClose();
    ~scoreDialog();

private:
    Ui::scoreDialog *ui;
    bool _close;

private slots:
    void setClose();
};

#endif // SCOREDIALOG_H
