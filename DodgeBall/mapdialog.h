#ifndef MAPDIALOG_H
#define MAPDIALOG_H

#include <QDialog>
#include <QGraphicsItem>
#include <QGraphicsScene>
#include <QTimer>
#include "ball.h"
#include "player.h"
#include "scoredialog.h"

namespace Ui {
class mapDialog;
}

class mapDialog : public QDialog
{
    Q_OBJECT

public:
    explicit mapDialog(int _uid, QWidget *parent = nullptr);
    void SetSocket(QTcpSocket* _sock);
    Player* getPlayer(int index);
    ~mapDialog();

protected:
    void keyPressEvent(QKeyEvent* e);
    void keyReleaseEvent(QKeyEvent* e);
    void closeEvent(QCloseEvent* e);

private:
    Ui::mapDialog *ui;
    QGraphicsScene* scene;
    QTcpSocket* socket;
    int myPlayer;
    int myBall;
    Player* playersUid[6] = {nullptr};
    Ball* dodgeballs[4] = {nullptr};
    QTimer* timer;
    QTimer timer_sec;
    int time;
    int throws;
    void throws_inc();
    QLine line;
    void sendPos();
    void sendBallInfo();
    scoreDialog *scores;
    bool inGame;

signals:
    void finishGame(QString);

private slots:
    void processMessage();
    void player_Hit();
    void time_inc();
};

#endif // MAPDIALOG_H
