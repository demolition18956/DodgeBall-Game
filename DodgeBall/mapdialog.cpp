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
    scene = new QGraphicsScene(-XMAX/2, -YMAX/2, XMAX, YMAX + 40, this);
    scene->setBackgroundBrush(QBrush(Qt::black));

    // Ensures the widget size cannot be changed
    showNormal();
    setFixedSize(XMAX + 100,YMAX + 100);

    // Disallows widget focus, sets widget center and enables antialiasing
    ui->graphicsView->setFocusPolicy(Qt::NoFocus);
    ui->graphicsView->centerOn(0, 0);
    ui->graphicsView->setRenderHints(QPainter::Antialiasing);

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
            //qDebug() << "up";
            playersUid[myPlayer-1]->move(PlayerDirection::up);
        }

        // If the key pressed is A or Left arrow, move player left
        else if(e->key()==Qt::Key_A || e->key()==Qt::Key_Left)
        {
            //qDebug() << "left";
            playersUid[myPlayer-1]->move(PlayerDirection::left);
        }

        // If the key pressed is S or Down arrow, move player upwards
        else if(e->key()==Qt::Key_S || e->key()==Qt::Key_Down)
        {
            //qDebug() << "down";
            playersUid[myPlayer-1]->move(PlayerDirection::down);
        }

        // If the key pressed is D or Right arrow, move player upwards
        else if(e->key()==Qt::Key_D || e->key()==Qt::Key_Right)
        {
            //qDebug() << "right";
            playersUid[myPlayer-1]->move(PlayerDirection::right);
        }

        // If the key pressed is the Space Bar, player will attempt to pick up a ball
        else if ((e->key()==Qt::Key_Space) && (playersUid[myPlayer-1]->isHoldingBall() == false))
        {
            if(playersUid[myPlayer-1]->isHoldingBall()){
                return;
            }
            else{
                //qDebug() << "pickup";
                playersUid[myPlayer-1]->move(PlayerDirection::pickup);
            }
        }
        else if ((e->key() == Qt::Key_C) && (playersUid[myPlayer-1]->isHoldingBall() == true)) //if we press space
        {
            //player will attempt to throw a ball if they have it
            playersUid[myPlayer - 1]->incrementThrows();
            if(playersUid[myPlayer-1]->getTeam() == "red" && playersUid[myPlayer-1]->isHoldingBall())
            {
                Ball *testBall = new Ball(playersUid[myPlayer-1]->GetX(),playersUid[myPlayer-1]->GetY());
                testBall->setMove(1);
                testBall->setMoving(true);
                int bid = playersUid[myPlayer-1]->ballHeld;
                dodgeballs[bid-1] = testBall;
                connect(dodgeballs[bid-1], SIGNAL(playerHit()), this, SLOT(player_Hit()));
                scene->addItem(dodgeballs[bid-1]);
                playersUid[myPlayer-1]->setHoldingBall(false); //make the player not hold ball anymore
                //setJustThrew to true
                playersUid[myPlayer-1]->setJustThrew(true);
                playersUid[myPlayer-1]->_throw = true;
                this->sendBallInfo();
            }
            else if(playersUid[myPlayer-1]->getTeam() == "blue" && playersUid[myPlayer-1]->isHoldingBall())
            {
                Ball *testBall = new Ball(playersUid[myPlayer-1]->GetX(),playersUid[myPlayer-1]->GetY());
                testBall->setMove(2);
                testBall->setMoving(true);
                int bid = playersUid[myPlayer-1]->ballHeld;
                dodgeballs[bid-1] = testBall;
                connect(dodgeballs[bid-1], SIGNAL(playerHit()), this, SLOT(player_Hit()));
                scene->addItem(dodgeballs[bid-1]);
                playersUid[myPlayer-1]->setHoldingBall(false);
                //setJustThrew to true
                playersUid[myPlayer-1]->setJustThrew(true);
                playersUid[myPlayer-1]->_throw = true;
                this->sendBallInfo();
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
            //qDebug() << "up";
            playersUid[myPlayer-1]->move(PlayerDirection::vstop);
        }

        // When the A or D, or Right arrow or Left arrow is released, stop the player from moving horizontally
        else if((e->key()==Qt::Key_A) || (e->key()==Qt::Key_D) || ((e->key()==Qt::Key_Left) || (e->key()==Qt::Key_Right)))
        {
            //qDebug() << "left";
            playersUid[myPlayer-1]->move(PlayerDirection::hstop);
        }

        // When the Space Bar is released, stop the player from picking up a ball
        else if (e->key()==Qt::Key_Space)
        {
            //qDebug() << "stoppick";
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
    //qDebug() << "BUFFER: " << buffer;
    //qDebug() << "WE are in via map";


    // Read Player Information (packet layout-->"PLAYER: uid team x y hasBall")
    while((buffer == "PLAYER:") || (buffer == "BALL:") || (buffer == "Hit:") || (buffer == "Finish:"))
    {
        static int ballCounter = 0;
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
            //qDebug() << "Received the UID:" << uid;
            buffer.clear();
            message >> buffer;  // team read
            team = buffer;
            //qDebug() << "Received the Team:" << team;
            buffer.clear();
            message >> buffer;   // x pos read
            x = buffer.toInt();
            //qDebug() << "Received the X:" << x;
            buffer.clear();
            message >> buffer;   // y pos read
            y = buffer.toInt();
            //qDebug() << "Received the Y:" << y;
            buffer.clear();
            message >> buffer;   // hasBall read
            hasBall = buffer.toInt();
            //qDebug() << "Received the HasBall" << hasBall;
            buffer.clear();
            if (playersUid[uid-1] == nullptr){
                //qDebug() << "yo";
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
                //qDebug() << "MAPDIALOG.CPP: SETTING X AND Y FOR OTHER PLAYERS";
                //qDebug() << x;
                //qDebug() << y;

                playersUid[uid-1]->setHoldingBall(hasBall);
                playersUid[uid-1]->SetX(x);   // setting x,y variables. Object is moved in advance()
                playersUid[uid-1]->SetY(y);
            }
            if(playersUid[uid-1]->updatePos)   // myPlayer updated position, will send to server from mapdialog socket
            {
                this->sendPos();
                //qDebug() << "Position Sent";
                playersUid[uid-1]->updatePos = false;
            }
            if(playersUid[uid-1]->grab)
            {
                this->sendBallInfo();
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
            //qDebug() << "Received the bid:" << bid;
            buffer.clear();
            message >> buffer;   // x pos read
            x = buffer.toInt();
            //qDebug() << "Received the X:" << x;
            buffer.clear();
            message >> buffer;   // y pos read
            y = buffer.toInt();
            //qDebug() << "Received the Y:" << y;
            buffer.clear();
            message >> buffer;   // isHeld read
            isHeld = buffer.toInt();
            //qDebug() << "Received the isHeld" << isHeld;
            buffer.clear();
            message >> buffer;   // team read
            team = buffer;
            //qDebug() << "Received the team" << team;
            buffer.clear();

            if( (playersUid[myPlayer-1]->ballHeld != bid) && dodgeballs[bid-1] && isHeld) // Remove a ball from the scene once it is picked up
            {
                //qDebug() << "CLIENT: REMOVING FROM SCENE";
                scene->removeItem(dodgeballs[bid-1]);
                delete dodgeballs[bid-1];
                dodgeballs[bid-1] = nullptr;
            }
            else if( (playersUid[myPlayer-1]->ballHeld != bid) && (dodgeballs[bid-1] == nullptr) && !isHeld)
            {   // Add a moving ball to the scene once it is throw
                dodgeballs[bid-1] = new Ball(x, y, this);
                connect(dodgeballs[bid-1], SIGNAL(playerHit()), this, SLOT(player_Hit()));
                dodgeballs[bid-1]->bid = bid;
                //qDebug() << "CLIENT: new Ball created";
                if(team == "red")
                {
                    dodgeballs[bid-1]->setMove(1);
                }
                else if(team == "blue")
                {
                    dodgeballs[bid-1]->setMove(2);
                }
                scene->addItem(dodgeballs[bid-1]);

                ballCounter++;
            }
        }
        else if(buffer == "Hit:") // Remove a player once they are hit with a ball
        {
            int uid;
            buffer.clear();
            message >> buffer;  // uid read
            uid = buffer.toInt();
            buffer.clear();
            //qDebug() << "CLIENT: Received the uid on HIT -> " << QString::number(uid);
            //qDebug() << "Removing Player";
            scene->removeItem(playersUid[uid-1]);
        }
        else if(buffer == "Finish:")
        {   // End the game when only one team is left standing
            timer->stop();
            delete timer;
            scene->clear();
            delete scene;
            QString team;
            buffer.clear();
            message >> buffer;  // team read
            team = buffer;
            buffer.clear();
            qDebug() << "CLIENT: Received the team on FINISH -> " << team;
            qDebug() << "GAME FINISHED!!!";
            disconnect(socket, SIGNAL(readyRead()),this, SLOT(processMessage()));
            emit this->finishGame(team);
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
        //qDebug() << "CLIENT: Position updated, Sending";
        QString msg = "Player: ";
        //qDebug() << "myPlayer: " << myPlayer;
        msg.append(QString::number(playersUid[myPlayer-1]->GetX()));
        //qDebug() << "got x";
        msg.append(" ");
        msg.append(QString::number(playersUid[myPlayer-1]->GetY()));
        //qDebug() << "got y";
        msg.append(" ");
        msg.append(QString::number(playersUid[myPlayer-1]->isHoldingBall()));
        //qDebug() << "got hasBall";
        //qDebug() << msg;

        QByteArray block;
        QTextStream out(&block, QIODevice::ReadWrite);
        out << msg << endl;

        socket->write(block);
        socket->flush();
        //qDebug() << "CLIENT: POSITION SENT";
    }
}

void mapDialog::sendBallInfo()
{
    if((playersUid[myPlayer-1] != nullptr) && (playersUid[myPlayer-1]->ballHeld != -1))
    {
        //qDebug() << "sendBllInfo: ";
        //qDebug() << "_THROW: " << playersUid[myPlayer-1]->_throw;
        //qDebug() << "GRAB: " << playersUid[myPlayer-1]->grab;

        if(playersUid[myPlayer-1]->grab)
        {
            //qDebug() << "CLIENT: BallHeld Ball Grabbed, Sending";  // packet: "Grab: bid"
            QString msg = "Grab: ";
            //qDebug() << "myPlayer: " << myPlayer;
            msg.append(QString::number(playersUid[myPlayer-1]->ballHeld));
            //qDebug() << "got ballHeld";

            //qDebug() << msg;

            QByteArray block;
            QTextStream out(&block, QIODevice::ReadWrite);
            out << msg << endl;

            //qDebug() << "block";
            //qDebug() << block;
            socket->write(block);
            socket->flush();
            //qDebug() << "CLIENT: Ball Grab Data SENT";
            playersUid[myPlayer-1]->grab = false;
        }

        if(playersUid[myPlayer-1]->_throw)  // just threw a ball
        {
            //qDebug() << "CLIENT: BallHeld Ball Thrown, Sending";  // packet: "Throw: bid x y team"
            QString msg = "Throw: ";
            //qDebug() << "myPlayer: " << myPlayer;
            msg.append(QString::number(playersUid[myPlayer-1]->ballHeld));
            //qDebug() << "got ballHeld";
            msg.append(" ");
            msg.append(QString::number(playersUid[myPlayer-1]->GetX()));
            //qDebug() << "got x";
            msg.append(" ");
            msg.append(QString::number(playersUid[myPlayer-1]->GetY()));
            //qDebug() << "got y";
            msg.append(" ");
            msg.append(playersUid[myPlayer-1]->getTeam());
            //qDebug() << "got team";

            //qDebug() << msg;

            QByteArray block;
            QTextStream out(&block, QIODevice::ReadWrite);
            out << msg << endl;

            socket->write(block);
            socket->flush();
            //qDebug() << "CLIENT: Ball Thrown Data SENT";
            playersUid[myPlayer-1]->ballHeld = -1;
            playersUid[myPlayer-1]->_throw = false;
        }
    }
}

void mapDialog::player_Hit()
{
    //qDebug() << "PLAYER HIT!!!!!";
    //qDebug() << "CLIENT: Player Hit, Sending Data";  // packet: "Hit: uid"
    QString msg = "Hit: ";
    //qDebug() << "myPlayer: " << myPlayer;

    msg.append(QString::number(myPlayer));

    //qDebug() << msg;

    QByteArray block;
    QTextStream out(&block, QIODevice::ReadWrite);
    out << msg << endl;

    socket->write(block);
    socket->flush();

    playersUid[myPlayer-1]->incrementKills();
    //qDebug() << "CLIENT: Player Hit Data Sent";
}
