#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>

class Player : public QTcpSocket
{

private:
    int x;   // the player's position
    int y;
    bool ready;   // is the player ready?
    bool hasBall;   // is the player holding a ball?
    void setReady(bool flag);

public:
    Player();
    int GetX();
    int GetY();
    bool isReady();
    bool isHoldingBall();
};

#endif // PLAYER_H
