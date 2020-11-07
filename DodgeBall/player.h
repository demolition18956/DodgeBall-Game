#ifndef PLAYER_H
#define PLAYER_H

#include <QTcpSocket>

class Player
{

private:
    QTcpSocket* socket;
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
    QTcpSocket* getSocket();
};

#endif // PLAYER_H
