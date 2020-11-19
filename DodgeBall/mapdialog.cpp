#include "mapdialog.h"
#include "ui_mapdialog.h"
#include <QKeyEvent>
#include "defs.h"

mapDialog::mapDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mapDialog)
{

    ui->setupUi(this);
    scene = new QGraphicsScene(-XMAX/2, -YMAX/2, XMAX, YMAX, this);
    scene->setBackgroundBrush(QBrush(Qt::black));
    //QRectF bounds = scene->itemsBoundingRect();
    //bounds.setWidth(bounds.width()*0.9);
    //bounds.setHeight(bounds.height()*0.9);
    //ui->graphicsView->fitInView(bounds, Qt::IgnoreAspectRatio);
    showNormal();
    ui->graphicsView->centerOn(0, 0);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);
    myPlayer = new Player(0,0,true,"red");
    Ball *testBall = new Ball(100,100);
    scene->addItem(testBall);
    scene->addItem(myPlayer);
    setFixedSize(XMAX + 100,YMAX + 100);
    QPen pen;
    pen.setColor(QColor(255,255,255));
    pen.setWidth(10);
    scene->addLine(0, YMAX/2, 0, -YMAX/2, pen);


    ui->graphicsView->setScene(scene);
    ui->graphicsView->show();

    timer = new QTimer(this);
    timer->setInterval(1000/33);
    timer->start();
    connect(timer, SIGNAL(timeout()), scene, SLOT(advance()));
}

mapDialog::~mapDialog()
{
    delete ui;
}

void mapDialog::keyPressEvent(QKeyEvent *e)
{
    if(e!=NULL){
        if(e->key()==Qt::Key_W)
        {
            qDebug() << "up";
            myPlayer->move(PlayerDirection::up);
        }
        else if(e->key()==Qt::Key_A)
        {
            qDebug() << "left";
            myPlayer->move(PlayerDirection::left);
        }
        else if(e->key()==Qt::Key_S)
        {
            qDebug() << "down";
            myPlayer->move(PlayerDirection::down);
        }
        else if(e->key()==Qt::Key_D)
        {
            qDebug() << "right";
            myPlayer->move(PlayerDirection::right);
        }
        else if (e->key()==Qt::Key_Space)
        {
            qDebug() << "pickup";
            myPlayer->move(PlayerDirection::pickup);
        }
    }
    QDialog::keyPressEvent(e);
}

void mapDialog::keyReleaseEvent(QKeyEvent *e)
{
    if (e!=NULL)
    {
        if((e->key()==Qt::Key_W) || (e->key()==Qt::Key_S))
        {
            qDebug() << "up";
            myPlayer->move(PlayerDirection::vstop);
        }
        else if((e->key()==Qt::Key_A) || (e->key()==Qt::Key_D))
        {
            qDebug() << "left";
            myPlayer->move(PlayerDirection::hstop);
        }
        else if (e->key()==Qt::Key_Space){
            qDebug() << "stoppick";
            myPlayer->move(PlayerDirection::stoppick);
        }
//       qDebug() << "stop";
//       myPlayer->move("stop");
    }
    QDialog::keyReleaseEvent(e);
}

void mapDialog::SetSocket(QTcpSocket *_sock)
{
    socket = _sock;
}
