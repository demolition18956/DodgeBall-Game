#include "player.h"

Player::Player()
{
    ready = false;
}

int Player::GetX()
{
    return x;
}

int Player::GetY()
{
    return y;
}

bool Player::isReady()
{
    return ready;
}

bool Player::isHoldingBall()
{
    return hasBall;
}

void Player::setReady(bool flag)
{
    ready = flag;
}

QTcpSocket* Player::getSocket()
{
    return socket;
}

void Player::setSocket(QTcpSocket *sock)
{
    socket = sock;
}

bool Player::isHost()
{
    return host;
}

Player::~Player()
{
    delete socket;
}
