#ifndef BALL_H
#define BALL_H

#include <QObject>
#include <QGraphicsItem>
#include <QPainter>
#include <QRectF>

class Ball : public QObject, public QGraphicsItem
{
    Q_OBJECT
public:
    explicit Ball(int _x, int _y, QObject *parent = nullptr);
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    void advance(int phase);
    void setMove(int a);
    int getMove();
    void SetX(int _x);
    void SetY(int _y);
    void setMoving(bool m);
    bool getMoving();

signals:

public slots:

private:
    int x;
    int y;
    int r;
    int move;
    QRectF ellipse;
    bool isMoving;
};

#endif // BALL_H
