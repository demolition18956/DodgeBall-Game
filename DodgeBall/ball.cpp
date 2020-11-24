#include "ball.h"

Ball::Ball(int _x, int _y, QObject *parent) : QObject(parent)
{
    x = _x;
    y = _y;
    r = 20;
    ellipse.setRect(x-r/2,y-r/2,r,r);
    move = 0;
}

QRectF Ball::boundingRect() const
{
    qreal adjust = 0.8;
    return QRectF(x-r/2-adjust, y-r/2-adjust, r+adjust, r+adjust);
}

void Ball::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->setPen(QPen(Qt::gray, 2, Qt::SolidLine, Qt::FlatCap));
    painter->setBrush(QBrush(Qt::gray, Qt::SolidPattern));
    painter->drawEllipse(ellipse);
}

void Ball::advance(int phase)
{
    if(phase == 0)
    {
        this->moveBy(0,0);
    }
    else {

        if (getMove() == 1) // if phase == 1, then its red team
        {
            this->moveBy(5,0);
        }
        else if(getMove() == 2) //blue team
        {
            this->moveBy(-5,0);
        }
    }
    return;
}

int Ball::getMove()
{
    return move;
}

//set the bool to whwere we can see if the player is red or blue team
void Ball::setMove(int a)
{
    move = a;
}
