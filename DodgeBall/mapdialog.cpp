#include "mapdialog.h"
#include "ui_mapdialog.h"
#include <QKeyEvent>
#include "defs.h"

mapDialog::mapDialog(int _uid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mapDialog)
{

    ui->setupUi(this);
    myPlayer = _uid;
    scene = new QGraphicsScene(-XMAX/2, -YMAX/2, XMAX, YMAX, this);
    scene->setBackgroundBrush(QBrush(Qt::black));
    //QRectF bounds = scene->itemsBoundingRect();
    //bounds.setWidth(bounds.width()*0.9);
    //bounds.setHeight(bounds.height()*0.9);
    //ui->graphicsView->fitInView(bounds, Qt::IgnoreAspectRatio);
    showNormal();
    ui->graphicsView->centerOn(0, 0);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);
    Ball *testBall = new Ball(100,100);
    scene->addItem(testBall);
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
        if(e->key()==Qt::Key_W || e->key()==Qt::Key_Up)
        {
            qDebug() << "up";
            playersUid[myPlayer]->move(PlayerDirection::up);
        }
        else if(e->key()==Qt::Key_A || e->key()==Qt::Key_Left)
        {
            qDebug() << "left";
            playersUid[myPlayer]->move(PlayerDirection::left);
        }
        else if(e->key()==Qt::Key_S || e->key()==Qt::Key_Down)
        {
            qDebug() << "down";
            playersUid[myPlayer]->move(PlayerDirection::down);
        }
        else if(e->key()==Qt::Key_D || e->key()==Qt::Key_Right)
        {
            qDebug() << "right";
            playersUid[myPlayer]->move(PlayerDirection::right);
        }
        else if (e->key()==Qt::Key_Space)
        {
            qDebug() << "pickup";
            playersUid[myPlayer]->move(PlayerDirection::pickup);
        }
    }
        QDialog::keyPressEvent(e);
}

void mapDialog::keyReleaseEvent(QKeyEvent *e)
{
    if (e!=NULL)
    {
        if((e->key()==Qt::Key_W) || (e->key()==Qt::Key_S) || ((e->key()==Qt::Key_Up) || (e->key()==Qt::Key_Down)))
        {
            qDebug() << "up";
            playersUid[myPlayer]->move(PlayerDirection::vstop);
        }
        else if((e->key()==Qt::Key_A) || (e->key()==Qt::Key_D) || ((e->key()==Qt::Key_Left) || (e->key()==Qt::Key_Right)))
        {
            qDebug() << "left";
            playersUid[myPlayer]->move(PlayerDirection::hstop);
        }
        else if (e->key()==Qt::Key_Space){
            qDebug() << "stoppick";
            playersUid[myPlayer]->move(PlayerDirection::stoppick);
        }
//       qDebug() << "stop";
//       myPlayer->move("stop");
    }
    QDialog::keyReleaseEvent(e);
}

void mapDialog::SetSocket(QTcpSocket *_sock)
{
    socket = _sock;
    connect(socket, SIGNAL(readyRead()),this, SLOT(processMessage()));
}

void mapDialog::processMessage()
{
    QTextStream message(socket);
    QString buffer;
    message >> buffer;
    qDebug() << "BUFFER: " << buffer;
    qDebug() << "WE are in via map";


    // Read Player Information (packet layout-->"PLAYER: uid team x y hasBall pixmap")
    if(buffer == "PLAYER:")
    {
        int uid;
        QString team;
        int x;
        int y;
        bool hasBall;
//            QByteArray pixmap;
        buffer.clear();
        message >> buffer;  // uid read
        uid = buffer.toInt();
        buffer.clear();
        message >> buffer;  // team read
        team = buffer;
        buffer.clear();
        message >> buffer;   // x pos read
        x = buffer.toInt();
        buffer.clear();
        message >> buffer;   // y pos read
        y = buffer.toInt();
        buffer.clear();
        message >> buffer;   // hasBall read
        hasBall = buffer.toInt();
        buffer.clear();
        if (playersUid[uid] == nullptr){
            qDebug() << "yo";
            playersUid[uid] = new Player(x,y,uid == myPlayer,team);
            scene->addItem(playersUid[uid]);
        }

    }
}
