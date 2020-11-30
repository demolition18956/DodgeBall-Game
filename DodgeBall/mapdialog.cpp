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
//    Ball *testBall = new Ball(100,100);
//    scene->addItem(testBall);
    //testBall->advance(0);


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
            playersUid[myPlayer-1]->move(PlayerDirection::up);
        }

        // If the key pressed is A or Left arrow, move player left
        else if(e->key()==Qt::Key_A || e->key()==Qt::Key_Left)
        {
            qDebug() << "left";
            playersUid[myPlayer-1]->move(PlayerDirection::left);
        }

        // If the key pressed is S or Down arrow, move player upwards
        else if(e->key()==Qt::Key_S || e->key()==Qt::Key_Down)
        {
            qDebug() << "down";
            playersUid[myPlayer-1]->move(PlayerDirection::down);
        }

        // If the key pressed is D or Right arrow, move player upwards
        else if(e->key()==Qt::Key_D || e->key()==Qt::Key_Right)
        {
            qDebug() << "right";
            playersUid[myPlayer-1]->move(PlayerDirection::right);
        }

        // If the key pressed is the Space Bar, player will attempt to pick up a ball
        else if (e->key()==Qt::Key_Space)
        {
            qDebug() << "pickup";
            playersUid[myPlayer-1]->move(PlayerDirection::pickup);
        }
        else if (e->key() == Qt::Key_C)//if we press c
        {
            //player will attempt to throw a ball if tehy have it
            if(playersUid[myPlayer-1]->getTeam() == "red" && playersUid[myPlayer-1]->isHoldingBall())
            {
                Ball *testBall = new Ball(playersUid[myPlayer-1]->GetX(),playersUid[myPlayer-1]->GetY());
                testBall->setMove(1);
                scene->addItem(testBall);
                playersUid[myPlayer-1]->setHoldingBall(false); //make the player not hold ball anymore

            }
            else if(playersUid[myPlayer-1]->getTeam() == "blue" && playersUid[myPlayer-1]->isHoldingBall())
            {
                Ball *testBall = new Ball(playersUid[myPlayer-1]->GetX(),playersUid[myPlayer-1]->GetY());
                testBall->setMove(2);
                scene->addItem(testBall);
                playersUid[myPlayer-1]->setHoldingBall(false);

            }
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
            playersUid[myPlayer-1]->move(PlayerDirection::vstop);
        }

        // When the A or D, or Right arrow or Left arrow is released, stop the player from moving horizontally
        else if((e->key()==Qt::Key_A) || (e->key()==Qt::Key_D) || ((e->key()==Qt::Key_Left) || (e->key()==Qt::Key_Right)))
        {
            qDebug() << "left";
            playersUid[myPlayer-1]->move(PlayerDirection::hstop);
        }

        // When the Space Bar is released, stop the player from picking up a ball
        else if (e->key()==Qt::Key_Space)
        {
            qDebug() << "stoppick";
            playersUid[myPlayer-1]->move(PlayerDirection::stoppick);
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


    // Read Player Information (packet layout-->"PLAYER: uid team x y hasBall")
    while((buffer == "PLAYER:") || (buffer == "BALL:"))
    {
        if(buffer == "PLAYER:")
        {
            int uid;
            QString team;
            int x;
            int y;
            bool hasBall;
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
            if (playersUid[uid-1] == nullptr){
                qDebug() << "yo";
                playersUid[uid-1] = new Player(x,y,uid == myPlayer,team);
                if(uid == myPlayer)
                {
                    if(playersUid[uid-1]->updatePos)   // myPlayer updated position, will send to server from mapdialog socket
                    {
                        this->sendPos();
                        playersUid[uid-1]->updatePos = false;
                    }
                }
                scene->addItem(playersUid[uid-1]);
            }
            else if(uid != myPlayer)   // movement of myPlayer is handled in advance()
            {
                qDebug() << "MAPDIALOG.CPP: SETTING X AND Y FOR OTHER PLAYERS";
                qDebug() << x;
                qDebug() << y;
                playersUid[uid-1]->SetX(x);   // setting x,y variables. Object is moved in advance()
                playersUid[uid-1]->SetY(y);
            }
            if(playersUid[uid-1]->updatePos)   // myPlayer updated position, will send to server from mapdialog socket
            {
                this->sendPos();
                qDebug() << "Position Sent";
                playersUid[uid-1]->updatePos = false;
            }
        }

        else if(buffer == "BALL:")
        {
            int bid;
            int x;
            int y;
            bool isHeld;
            QString team;
            buffer.clear();
            message >> buffer;  // bid read
            bid = buffer.toInt();
            qDebug() << "Received the bid:" << bid;
            buffer.clear();
            message >> buffer;   // x pos read
            x = buffer.toInt();
            qDebug() << "Received the X:" << x;
            buffer.clear();
            message >> buffer;   // y pos read
            y = buffer.toInt();
            qDebug() << "Received the Y:" << y;
            buffer.clear();
            message >> buffer;   // isHeld read
            isHeld = buffer.toInt();
            qDebug() << "Received the isHeld" << isHeld;
            buffer.clear();
            message >> buffer;   // isHeld read
            team = buffer;
            qDebug() << "Received the team" << team;
            buffer.clear();

            if(dodgeballs[bid-1] == nullptr)
            {
                dodgeballs[bid-1] = new Ball(x, y, this);
                qDebug() << "CLIENT: new Ball created";
                scene->addItem(dodgeballs[bid-1]);
            }
            else
            {
                dodgeballs[bid-1]->SetX(x);
                dodgeballs[bid-1]->SetY(y);
            }
        }
        message.readLine();
        message >> buffer;
    }
}

//*************************************************************************************************//
//                               Send Position of myPlayer                                         //
//*************************************************************************************************//
void mapDialog::sendPos()   // packet template: "Player: x y hasBall"
{
    if(playersUid[myPlayer-1] != nullptr)
    {
        qDebug() << "CLIENT: Position updated, Sending";
        QString msg = "Player: ";
        qDebug() << "myPlayer: " << myPlayer;
        msg.append(QString::number(playersUid[myPlayer-1]->GetX()));
        qDebug() << "got x";
        msg.append(" ");
        msg.append(QString::number(playersUid[myPlayer-1]->GetY()));
        qDebug() << "got y";
        msg.append(" ");
        msg.append(QString::number(playersUid[myPlayer-1]->isHoldingBall()));
        qDebug() << "got hasBall";
        qDebug() << msg;

        QByteArray block;
        QTextStream out(&block, QIODevice::ReadWrite);
        out << msg << endl;

        socket->write(block);
        socket->flush();
        qDebug() << "CLIENT: POSITION SENT";
    }
}
