#include "player.h"

Player::Player()
{
    ready = false;
    socket = new QTcpSocket();
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

bool Player::isHost()
{
    return host;
}

Player::~Player()
{
    delete socket;
}
