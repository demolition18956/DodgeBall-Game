#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>
#include <QGraphicsItem>
#include <QPainter>
#include <QBrush>
#include <QTimer>
#include <QPointF>
#include <QObject>
#include <QRectF>
#include <QPixmap>
#include <QPainterPath>
#include "defs.h"

class Player : public QObject, public QGraphicsItem
{

public:
    Player(int _x, int _y, bool _isUser, QString _team);
    QRectF boundingRect() const;
    void advance(int);
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    int GetX();
    int GetY();
    void SetX(int);
    void SetY(int);
    bool isReady();
    bool isHoldingBall();
    bool isHost();
    void move(PlayerDirection);
    void setHoldingBall(bool a);
    QString getTeam();
    bool updatePos;
    bool getHoldingBall();
    void setJustThrew(bool a);
    bool getJustThrew();
    int ballHeld;  // bid of the ball player is holding. will be -1 when no ball held
    bool grab;
    bool _throw;
    bool getIsPlayer();
    ~Player();

private:
    int x;   // the player's position
    int y;
    int w;  // the player's size
    int h;
    int dx;
    int dy;
    bool ready;   // is the player ready?
    bool hasBall;   // is the player holding a ball?
    bool host;   // is this player the host?
    void setReady(bool flag);
    bool isUser;
    QPixmap teamColors;
    QPainterPath heart;
    bool ballAttempt;
    QString team;
    bool justThrew;
    int delay;


    //QPointF starpts[10];
};

#endif // PLAYER_H
