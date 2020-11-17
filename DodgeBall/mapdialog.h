#ifndef MAPDIALOG_H
#define MAPDIALOG_H

#include <QDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>

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
    QGraphicsScene* scene;
};

#endif // MAPDIALOG_H
