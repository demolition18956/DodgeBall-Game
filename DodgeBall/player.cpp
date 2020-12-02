#include "player.h"
#include <QtDebug>
#include <QGraphicsScene>
#include "ball.h"

Player::Player(int _x, int _y, bool _isUser, QString _team) :
    QGraphicsItem()
{
    delay = 0;
    ballHeld = -1;
    grab = false;
    _throw = false;
    w = 80;
    h = 80;
    x = _x;
    y = _y;
    dx = 0;
    dy = 0;
    justThrew = false; // flag used for collision with ball
    ballAttempt = false;
    hasBall = false;
    isUser = _isUser;
    team = _team;
    if (team == "red")
    {
        teamColors.load(":/pixmaps/Red.png");
    } else if (team == "blue")
    {
        teamColors.load(":/pixmaps/Blue.png");
    }
    if (isUser)
    {
        heart.moveTo(0,h*1.0/4.0);
        heart.cubicTo(-w/4.0,-h*1.0/4.0,0,-h/4.0,0,-h/16.0);
        heart.moveTo(0,h*1.0/4.0);
        heart.cubicTo(w/4.0,-h*1.0/4.0,0,-h/4.0,0,-h/16.0);
    }
    this->setPos(x, y);
}

QRectF Player::boundingRect() const
{
    qreal adjust = 0.8;
    return QRectF(-w/2-adjust, -h/2-adjust, w+adjust, h+adjust);
}

void Player::advance(int phase)
{
    if(phase==0){
        return;
    }
    else
    {
        if(isUser)
        {
            if(this->getJustThrew())
            {
                delay++;
                if(delay > 15)
                {
                    justThrew = false;
                    delay = 0;
                }
            }
            this->moveBy(dx,dy);
            QList<QGraphicsItem*> collisions = this->collidingItems();
            //qDebug() << collisions.size();
            if (collisions.size()>0)
            {
                foreach(QGraphicsItem *i, collisions)
                {
                    if ((ballAttempt) && !(this->isHoldingBall())){
                        Ball* b = dynamic_cast<Ball *>(i);
                        if(b && !b->getMoving())
                        {
                            //qDebug() << b->type();
                            this->scene()->removeItem(b);
                            hasBall = true;
                            ballHeld = b->bid;
                            delete b;
                            //qDebug() << "CLIENT: PICKING UP BALL";
                            grab = true;
                            continue;
                        }
                    }
                    else
                    {
                        Ball* b = dynamic_cast<Ball *>(i);
                        if(b and !this->getJustThrew() and (b->getMove() > 0))
                        {
                            //b->setMove(0);
                            emit b->playerHit();
                            //qDebug() << "EMITTING PLAYER HIT!!!";
                        }
                    }

                    QGraphicsLineItem* line = dynamic_cast<QGraphicsLineItem *>(i);
                    if (line)
                    {
                        this->moveBy(-dx,0);
                    }

                }
            }

            if ((this->pos().y() < -YMAX/2 + h/2) || (this->pos().y() > YMAX/2 - h/2)){
                //qDebug() << "Max Y Hit";
                this->moveBy(0,-dy);
            }
            if ((this->pos().x() < -XMAX/2 + w/2) || (this->pos().x() > XMAX/2 - w/2)){
                //qDebug() << "Max X Hit";
                this->moveBy(-dx,0);
            }
            x = this->pos().x();
            y = this->pos().y();
            updatePos = true;
        }
        else {
            this->setPos(x,y);   // for showing other players (!isUser)
            //qDebug() << "PLAYER.CPP: MOVING OTHER PLAYER";
            //qDebug() << x;
            //qDebug() << y;
        }
    }
}

void Player::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QColor outline;
    if (hasBall){
        outline = Qt::yellow;
    }
    else{
        outline = Qt::white;
    }
    painter->drawPixmap(-w/2,-h/2,w,h,teamColors);
    painter->setPen(QPen(outline, 5, Qt::SolidLine, Qt::FlatCap));
    painter->drawRect((-w/2),(-h/2),w-1,h-1);
    if (isUser)
    {
        painter->setPen(QPen(Qt::black, 2, Qt::SolidLine, Qt::FlatCap));
        painter->setBrush(QBrush(Qt::black, Qt::SolidPattern));
        painter->drawPath(heart);
    }

    //painter->drawPolygon(starpts,10);
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

}

void Player::move(PlayerDirection dir)
{
    if(dir==PlayerDirection::up)
    {
//        dx=0;
        dy=-5;
    }
    else if(dir==PlayerDirection::left)
    {
        dx=-5;
//        dy=0;
    }
    else if(dir==PlayerDirection::down)
    {
//        dx=0;
        dy=5;
    }
    else if(dir==PlayerDirection::right)
    {
        dx=5;
//        dy=0;
    }
    else if(dir==PlayerDirection::vstop)
    {
        //leave cat running it same direction it was
        dy=0;
    }
    else if(dir==PlayerDirection::hstop)
    {
        //leave cat running it same direction it was
        dx=0;
    } else if (dir==PlayerDirection::pickup)
    {
        ballAttempt = true;
    }
    else if (dir == PlayerDirection::stoppick)
    {
        ballAttempt = false;
    }
}


void Player::SetX(int X)
{
    x = X;
}

void Player::SetY(int Y)
{
    y = Y;
}

QString Player::getTeam()
{
    return team;
}

void Player::setHoldingBall(bool a)
{
    hasBall = a;
}

bool Player::getHoldingBall()
{
    return hasBall;
}
//setters and getters for JustThrew
void Player::setJustThrew(bool a)
{
    justThrew = a;
}

bool Player::getJustThrew()
{
    return justThrew;
}

bool Player::getIsPlayer()
{
    return isUser;
}
