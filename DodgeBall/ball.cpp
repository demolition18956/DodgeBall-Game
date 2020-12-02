#include "ball.h"
#include "player.h"
#include <QGraphicsScene>

Ball::Ball(int _x, int _y, QObject *parent) : QObject(parent)
{
    x = _x;
    y = _y;
    r = 20;
    ellipse.setRect(x-r/2,y-r/2,r,r);
    move = 0;
    isMoving = false; //flag to know if ball is moving or not
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
        //collision detection with players
        /*
         * The justThrew variable is needed because as soon as the player throws the isHolding flag is set to false, which means
         * that the player throwing was detected as "hit" too so the only way I found to fix this is with this new flag on the player object
         * Also this removes the player from the scene but doesnt delete the object itself, I didnt want to try anything like that before
         * really understanding how to manage the database
         * */
        QList<QGraphicsItem*> collisions = this->collidingItems();
        foreach(QGraphicsItem *i, collisions)
        {
            Player* player = dynamic_cast<Player *>(i);
            if(this->getMoving())
            {
                if(player and !player->getHoldingBall() and !player->getJustThrew())
                {
                    //this->scene()->removeItem(player);
                    this->setMove(0);
                    if(player->getIsPlayer())  // only want to report when user is hit to avoid all players reporting each colission
                    {
                        emit playerHit();
                    }
                }
            }
        }
        if (getMove() == 1) // if phase == 1, then its red team
        {
            this->moveBy(10,0);
        }
        else if(getMove() == 2) //blue team
        {
            this->moveBy(-10,0);
        }

        if ((this->pos().x() < -XMAX/2) || (this->pos().x() > XMAX/2)){

            setMove(0);
            setMoving(false);
            this->moveBy(0,0);
            //phase = 0;

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

void Ball::SetX(int _x)
{
    x = _x;
}

void Ball::SetY(int _y)
{
    y = _y;
}

void Ball::setMoving(bool m)
{
    isMoving = m;
}

bool Ball::getMoving(){
    return isMoving;
}
