#include "mapdialog.h"
#include "ui_mapdialog.h"
#include <QKeyEvent>
#include "defs.h"

//*************************************************************************************************//
//                                      Constructor                                                //
//*************************************************************************************************//

mapDialog::mapDialog(int _uid, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::mapDialog)
{

    ui->setupUi(this);
    myPlayer = _uid;

    // Set scene dimensions and background color
    scene = new QGraphicsScene(-XMAX/2, -YMAX/2, XMAX, YMAX, this);
    scene->setBackgroundBrush(QBrush(Qt::black));

    // Ensures the widget size cannot be changed
    showNormal();
    setFixedSize(XMAX + 100,YMAX + 100);

    // Disallows widget focus, sets widget center and enables antialiasing
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->graphicsView->centerOn(0, 0);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);

    // Spawns a test ball
    Ball *testBall = new Ball(100,100);
    scene->addItem(testBall);

    // Draws the center line that cannot be crossed
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

//*************************************************************************************************//
//                                      Key Press Events                                           //
//*************************************************************************************************//

void mapDialog::keyPressEvent(QKeyEvent *e)
{
    // If a key is pressed
    if(e!=NULL)
    {
        // If the key pressed is W or Up arrow, move player upwards
        if(e->key()==Qt::Key_W || e->key()==Qt::Key_Up)
        {
            qDebug() << "up";
            playersUid[myPlayer]->move(PlayerDirection::up);
        }

        // If the key pressed is A or Left arrow, move player left
        else if(e->key()==Qt::Key_A || e->key()==Qt::Key_Left)
        {
            qDebug() << "left";
            playersUid[myPlayer]->move(PlayerDirection::left);
        }

        // If the key pressed is S or Down arrow, move player upwards
        else if(e->key()==Qt::Key_S || e->key()==Qt::Key_Down)
        {
            qDebug() << "down";
            playersUid[myPlayer]->move(PlayerDirection::down);
        }

        // If the key pressed is D or Right arrow, move player upwards
        else if(e->key()==Qt::Key_D || e->key()==Qt::Key_Right)
        {
            qDebug() << "right";
            playersUid[myPlayer]->move(PlayerDirection::right);
        }

        // If the key pressed is the Space Bar, player will attempt to pick up a ball
        else if (e->key()==Qt::Key_Space)
        {
            qDebug() << "pickup";
            playersUid[myPlayer]->move(PlayerDirection::pickup);
        }
    }
        QDialog::keyPressEvent(e);
}

//*************************************************************************************************//
//                                      Key Release Events                                         //
//*************************************************************************************************//

void mapDialog::keyReleaseEvent(QKeyEvent *e)
{
    if (e!=NULL)
    {
        // When the W or S, or Up arrow or Down arrow is released, stop the player from moving vertically
        if((e->key()==Qt::Key_W) || (e->key()==Qt::Key_S) || ((e->key()==Qt::Key_Up) || (e->key()==Qt::Key_Down)))
        {
            qDebug() << "up";
            playersUid[myPlayer]->move(PlayerDirection::vstop);
        }

        // When the A or D, or Right arrow or Left arrow is released, stop the player from moving horizontally
        else if((e->key()==Qt::Key_A) || (e->key()==Qt::Key_D) || ((e->key()==Qt::Key_Left) || (e->key()==Qt::Key_Right)))
        {
            qDebug() << "left";
            playersUid[myPlayer]->move(PlayerDirection::hstop);
        }

        // When the Space Bar is released, stop the player from picking up a ball
        else if (e->key()==Qt::Key_Space)
        {
            qDebug() << "stoppick";
            playersUid[myPlayer]->move(PlayerDirection::stoppick);
        }
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
    while(buffer == "PLAYER:")
    {
        int uid;
        QString team;
        int x;
        int y;
        bool hasBall;
//      QByteArray pixmap;
        buffer.clear();
        message >> buffer;  // uid read
        uid = buffer.toInt();
        qDebug() << "Received the UID:" << uid;
        buffer.clear();
        message >> buffer;  // team read
        team = buffer;
        qDebug() << "Received the Team:" << team;
        buffer.clear();
        message >> buffer;   // x pos read
        x = buffer.toInt();
        qDebug() << "Received the X:" << x;
        buffer.clear();
        message >> buffer;   // y pos read
        y = buffer.toInt();
        qDebug() << "Received the Y:" << y;
        buffer.clear();
        message >> buffer;   // hasBall read
        hasBall = buffer.toInt();
        qDebug() << "Received the HasBall" << hasBall;
        buffer.clear();
        if (playersUid[uid] == nullptr){
            qDebug() << "yo";
            playersUid[uid] = new Player(x,y,uid == myPlayer,team);
            scene->addItem(playersUid[uid]);
        }
        message.readLine();
        message >> buffer;

    }
}
