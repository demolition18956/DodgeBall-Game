#ifndef MAPDIALOG_H
#define MAPDIALOG_H

#include <QDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTimer>
#include "ball.h"
#include "player.h"

namespace Ui {
class mapDialog;
}

class mapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mapDialog(QWidget *parent = nullptr);
    void SetSocket(QTcpSocket* _sock);
    ~mapDialog();

protected:
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);

private:
    Ui::mapDialog *ui;
    QGraphicsScene* scene;
    QTcpSocket* socket;
    Player* myPlayer;
    QTimer* timer;
};

#endif // MAPDIALOG_H
