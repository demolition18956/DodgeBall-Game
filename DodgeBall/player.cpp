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
