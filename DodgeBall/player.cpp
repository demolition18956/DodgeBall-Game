#include "player.h"
#include <QtDebug>
#include <QGraphicsScene>
#include "ball.h"

Player::Player(int _x, int _y, bool _isUser, QString _team) :
    QGraphicsItem()
{
    w = 80;
    h = 80;
    x = _x;
    y = _y;
    dx = 0;
    dy = 0;
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
//    starpts[0] = QPointF(x-w/2+(6.5/14 * (float)w),y-h/2+(0.0/14 * h));
//    starpts[1] = QPointF(x-w/2+(9.0/14 * (float)w),y-h/2+(5.0/14 * (float)h));
//    starpts[2] = QPointF(x-w/2+(14.0/14 * (float)w),y-h/2+(5.5/14 * (float)h));
//    starpts[3] = QPointF(x-w/2+(10.5/14 * (float)w),y-h/2+(9.0/14 * (float)h));
//    starpts[4] = QPointF(x-w/2+(12.0/14 * (float)w),y-h/2+(float)h);
//    starpts[5] = QPointF(x-w/2+(6.5/14 * (float)w),y-h/2+(11.5/14 * (float)h));
//    starpts[6] = QPointF(x-w/2+(2.0/14 * (float)w),y-h/2+(14.0/14 * (float)h));
//    starpts[7] = QPointF(x-w/2+(3.5/14 * (float)w),y-h/2+(9.0/14 * (float)h));
//    starpts[8] = QPointF(x-w/2+(0.0/14 * (float)w),y-h/2+(5.5/14 * (float)h));
//    starpts[9] = QPointF(x-w/2+(5.0/14 * (float)w),y-h/2+(5.0/14 * (float)h));

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
            this->moveBy(dx,dy);
            QList<QGraphicsItem*> collisions = this->collidingItems();
            qDebug() << collisions.size();
            if (collisions.size()>0)
            {
                foreach(QGraphicsItem *i, collisions)
                {
                    if (ballAttempt){
                        Ball* b = dynamic_cast<Ball *>(i);
                        if(b)
                        {
                            qDebug() << b->type();
                            this->scene()->removeItem(b);
                            hasBall = true;
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
                qDebug() << "Max Y Hit";
                this->moveBy(0,-dy);
            }
            if ((this->pos().x() < -XMAX/2 + w/2) || (this->pos().x() > XMAX/2 - w/2)){
                qDebug() << "Max X Hit";
                this->moveBy(-dx,0);
            }
            x = this->pos().x();
            y = this->pos().y();
            updatePos = true;
        }
        else {
            this->setPos(x,y);   // for showing other players (!isUser)
            qDebug() << "PLAYER.CPP: MOVING OTHER PLAYER";
            qDebug() << x;
            qDebug() << y;
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
