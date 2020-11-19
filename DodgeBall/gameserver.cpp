#include "gameserver.h"
#include <QTcpSocket>
#include <QByteArray>
#include <QPixmap>
#include <QFile>

GameServer::GameServer(QObject* parent) :
    QTcpServer(parent)
{
    setMaxPendingConnections(6);
    connect(this, &QTcpServer::newConnection, this, &GameServer::ProcessNewConnections);
    if(!this->listen(QHostAddress::Any,2224))
    {
        qDebug() << "SERVER: Could not start server";
    }
    else
    {
        qDebug() << "SERVER: Listening";
    }

    playerCount = 0;
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open())
    {
            qDebug() << db.lastError();
            qDebug() << "Error: Unable to connect due to above error";
    }
    QSqlQuery setup;

    setup.exec("CREATE TABLE players(UID INT, playername TEXT, ready INT, ip TEXT, check(UID<=6))");
    if (!setup.isActive())
    {
        qDebug() << setup.lastError();
        qDebug() << "Creating Table 1 Error: unable to complete query";
        return;
    }
    setup.exec("CREATE TABLE sockets(UID INT check(UID<=6))");
    if (!setup.isActive())
    {
        qDebug() << setup.lastError();
        qDebug() << "Creating Table 2 Error: unable to complete query";
        return;
    }
    setup.exec("INSERT INTO sockets VALUES(1)");
    if (!setup.isActive())
    {
        qDebug() << setup.lastError();
        qDebug() << "add 1 Error: unable to complete query";
        return;
    }
    setup.exec("INSERT INTO sockets VALUES(2)");
    if (!setup.isActive())
    {
        qDebug() << setup.lastError();
        qDebug() << "add 2 Error: unable to complete query";
        return;
    }
    setup.exec("INSERT INTO sockets VALUES(3)");
    setup.exec("INSERT INTO sockets VALUES(4)");
    setup.exec("INSERT INTO sockets VALUES(5)");
    setup.exec("INSERT INTO sockets VALUES(6)");

    if (!setup.isActive())
    {
        qDebug() << setup.lastError();
        qDebug() << "Error: unable to complete query";
        return;
    }
    timer = new QTimer(this);
}

void GameServer::ProcessNewConnections()
{
    qDebug() << "Signal Emitted";
    while(hasPendingConnections())
    {
        qDebug() << "SERVER: processing incoming connection";
        if(playerCount >= 6)
        {
            QTcpSocket* sock = nextPendingConnection();
            sock->write("0");   // 0 for Rejected
            sock->abort();
            delete sock;
            qDebug() << "SERVER: player rejected!";
            return;
        }
        QTcpSocket* sock = this->nextPendingConnection();

        int sNum = getMinSocket();
        sock->write(QByteArray::number(sNum+1) + QByteArray("\n"));
        QSqlQuery qqqq;
        qqqq.prepare("INSERT INTO players(UID, playername, ready, ip) VALUES(?, ?, ?, ?)");
        qqqq.addBindValue(sNum+1);
        qqqq.addBindValue("Player " + QString::number(++playerCount));
        qDebug() << "Player Count " << playerCount;
        qDebug() << "Player " + QString::number(playerCount);
        int dfault = 0;
        qqqq.addBindValue(dfault);
        qqqq.addBindValue(sock->peerAddress().toString());
        qDebug() << sock->peerAddress().toString();
        if (!qqqq.exec())
        {
           qDebug() << qqqq.lastError();
           qDebug() << "Error on INSERT";
        }
        playerSockets[sNum] = sock;
        this->UpdateClients();
        connect(playerSockets[sNum], &QTcpSocket::readyRead,this, &GameServer::ReportReady);
        connect(playerSockets[sNum], &QTcpSocket::stateChanged, this, &GameServer::clientDisconnected);
    }
}

GameServer::~GameServer()
{

}

void GameServer::UpdateClients() {

    QSqlQuery socks;
    if (!socks.exec("SELECT UID FROM players"))
    {
       qDebug() << socks.lastError();
       qDebug() << "Error on INSERT";

    }
    qDebug() << "Updating";
    while(socks.next())
    {
        int playNum = 0;
        qDebug() << "Round";
        QTcpSocket* sock = playerSockets[socks.value(0).toInt()-1];
        qDebug() << sock;
        QByteArray block;
        QTextStream out(&block, QIODevice::WriteOnly);
        QSqlQuery info("SELECT playername, ready FROM players");
        while (info.next())
        {
            out << "Number: " << QString::number(++playNum) << endl
                << "Player Name: " << info.value(0).toString() << endl
                << "Ready: " << QString::number(info.value(1).toInt()) << endl;
        }
        while (playNum < 6)
        {
            out << "Number: " << QString::number(++playNum) << endl
                << "Player Name: No Player" << endl
                << "Ready: 0" << endl;
        }
        QSqlQuery ready("SELECT COUNT(ready) FROM players");
        if (ready.next())
        {
            out << "Joined: " << QString::number(ready.value(0).toInt()) << endl;
        }
        sock->write(block);
        qDebug() << block;

    }
}

int GameServer::getMinSocket()
{
    QSqlQuery q;
    if (!q.exec("SELECT MIN(UID) FROM sockets WHERE UID NOT IN (SELECT UID FROM players)"))
    {
        qDebug() << q.lastError();
        qDebug() << "Error on MIN";
    }
    while (q.next())
    {
        return (q.value(0).toInt() - 1);
    }
    return -1;
}

void GameServer::ReportReady()
{
    qDebug() << "Updating";
    QString str;
    int ind;
    for (int i = 0; i < 6; i++)
    {
        qDebug() << i;
        if ((playerSockets[i] != nullptr) && playerSockets[i]->canReadLine())
        {
            qDebug() << "we're in";
            QTextStream in(playerSockets[i]);
            while (in.readLineInto(&str))
            {
                qDebug() << str;
                if ((ind = str.indexOf(" Ready")) != -1)
                {
                    qDebug() << str.left(1);
                    QSqlQuery qq;
                    if (!qq.exec("UPDATE players SET ready = 1 WHERE UID=" + str.left(1)))
                    {
                        qDebug() << qq.lastError();
                        qDebug() << "Error on UPDATE";

                     }
                    if (!qq.exec("SELECT COUNT(UID) FROM players WHERE ready=1"))
                    {
                        qDebug() << qq.lastError();
                        qDebug() << "Error on SELECT";
                    }
                    qq.next();
                    qDebug() << qq.value(0).toInt();
                    if((qq.value(0).toInt() == playerCount) && playerCount >= 1)
                    {
                        qDebug() << "SERVER: Starting Game " << qq.value(0).toInt();
                        StartGame();
                    }
                }
                else if ((ind = str.indexOf(" NotReady")) != -1)
                {
                    qDebug() << str.left(1);
                    QSqlQuery qq;
                    if (!qq.exec("UPDATE players SET ready = 0 WHERE UID=" + str.left(1)))
                    {
                        qDebug() << qq.lastError();
                        qDebug() << "Error on UPDATE";

                     }
                }
                else if ((ind = str.indexOf(" Name ")) != -1)
                {
                    ind += 6;
                    QSqlQuery qq;
                    if (!qq.exec("UPDATE players SET playername = \""
                                 + str.right(str.length() - ind)
                                 + "\" WHERE UID="+str.left(1)))
                    {
                        qDebug() << qq.lastError();
                        qDebug() << "Error on UPDATE";
                    }
                }
            }
            this->UpdateReady();
            break;
        }
    }
}
void GameServer::UpdateReady()
{

    QSqlQuery socks;
    if (!socks.exec("SELECT UID FROM players")) {
       qDebug() << socks.lastError();
       qDebug() << "Error on INSERT";

    }

    while(socks.next())
    {
        int playNum = 0;
        QTcpSocket* sock = playerSockets[socks.value(0).toInt()-1];
        qDebug() << sock;
        QByteArray block;
        QTextStream out(&block, QIODevice::WriteOnly);
        QSqlQuery info("SELECT playername, ready FROM players");
        while (info.next()){
            out << "Number: " << QString::number(++playNum) << endl
                << "Player Name: " << info.value(0).toString() << endl
                << "Ready: " << QString::number(info.value(1).toInt()) << endl;
        }

        sock->write(block);
        qDebug() << block;


    }
}

void GameServer::clientDisconnected()
{
    for(int i=0;i<6;i++)
    {
        qDebug() << playerSockets[i];
        if(playerSockets[i] != nullptr)
        {
            qDebug() << playerSockets[i]->state();
            if(playerSockets[i]->state() == QAbstractSocket::ConnectedState)
            {
                continue;
            }
            else if (playerSockets[i]->state() == QAbstractSocket::UnconnectedState)
            {
                playerSockets[i]->deleteLater();
                playerSockets[i] = nullptr;
                playerCount--;
                // SQL HERE
                QSqlQuery del;
                if (!del.exec("DELETE FROM players WHERE UID=" + QString::number(i+1)))
                {
                    qDebug() << del.lastError();
                    qDebug() << "Error on DELETE";
                }
                this->UpdateClients();
                qDebug() << "Done";
            }
        }
    }
}

void GameServer::StartGame()
//send a message to start the game
{
    qDebug() << "SERVER: Starting Game";
    this->sendAll("start");
    QSqlQuery start;
//    if(!start.exec("CREATE TABLE sprites(pixmaps BLOB, team TEXT, hasBall INT)"))
//    {
//        qDebug() << start.lastError();
//        qDebug() << "Error on CREATE";
//        return;
//    }
    if(!start.exec("CREATE TABLE in_game(UID INT, x INT, y INT, hasBall INT, team TEXT)"))
    {
        qDebug() << start.lastError();
        qDebug() << "Error on CREATE";
    }
//    QByteArray red;
//    QFile in("../DodgeBall/pixmaps/Red.png");
//    if(!in.open(QIODevice::ReadOnly))
//    {
//        qDebug() << in.errorString();
//    }
//    red = in.readAll();
//    in.close();
//    in.setFileName("../DodgeBall/pixmaps/Blue.png");
//    QByteArray blue;
//    if(!in.open(QIODevice::ReadOnly))
//    {
//        qDebug() << in.errorString();
//    }
//    blue = in.readAll();
//    in.close();

//    qDebug() << "Red array: " << red;
//    qDebug() << "Blue array: " << blue;

    QSqlQuery q;
//    q.prepare("INSERT INTO sprites VALUES( :pixmap, :team, :hasBall )");
//    q.bindValue(":pixmap", red);
//    q.bindValue(":team", "red");
//    q.bindValue(":hasBall", 0);
//    if(!q.exec())
//    {
//        qDebug() << start.lastError();
//        qDebug() << "Error on INSERT";
//    }
//    q.clear();
//    q.prepare("INSERT INTO sprites VALUES( :pixmap, :team, :hasBall )");
//    q.bindValue(":pixmap", blue);
//    q.bindValue(":team", "blue");
//    q.bindValue(":hasBall", 0);
//    if(!q.exec())
//    {
//        qDebug() << start.lastError();
//        qDebug() << "Error on INSERT";
//    }
//    q.clear();
    if(!q.exec("SELECT UID FROM players"))  // now going to populate the in_game table
    {
        qDebug() << start.lastError();
        qDebug() << "Error on SELECT";
    }
    QString team = "red";
    while(q.next())
    {
        if(team == "red")
        {
            team = "blue";
        }
        else
        {
            team = "red";
        }
        QSqlQuery qq;
        qq.prepare("INSERT INTO in_game VALUES(:uid, :x, :y, :hasBall, :team)");
        qq.bindValue(":uid", q.value(0).toInt());
        qq.bindValue(":x", 0);
        qq.bindValue(":y", 0);
        qq.bindValue(":hasBall", 0);
        qq.bindValue(":team", team);
        if(!qq.exec())
        {
            qDebug() << start.lastError();
            qDebug() << "Error on INSERT";
        }
        qq.clear();
    }
    timer->start(250);
    connect(timer, &QTimer::timeout, this, &GameServer::onTimeout);
    qDebug() << "SERVER: Timer Started!";
}

//function that sends a message to everybody
void GameServer::sendAll(QString message)
{
        qDebug() << "Sending message to everybody";

       for(int i = 0; i < 6; i++){
            if((playerSockets[i] != nullptr) && (playerSockets[i]->state() == QAbstractSocket::ConnectedState)){
                qDebug() << playerSockets[i];
                QByteArray block;
                QTextStream out(&block, QIODevice::ReadWrite);
                out << message << endl;
                playerSockets[i]->write(block);
                playerSockets[i]->flush();
            }

        }
}

void GameServer::onTimeout()
{
    QSqlQuery q;
    if(!q.exec("SELECT UID FROM in_game"))
    {
        qDebug() << q.lastError();
        qDebug() << "Error on SELECT";
        timer->stop();
    }
    while(q.next())
    {
        QSqlQuery qq;
        QString msg;
        QString team;
        int x;
        int y;
        bool hasBall;
//        QByteArray ba;
        int uid;
        uid = q.value(0).toInt();
        qDebug() << "Got the UID: " << uid;
        qq.prepare("SELECT team FROM in_game WHERE UID=:uid");
        qq.bindValue(":uid",uid);
        if(!qq.exec())
        {
            qDebug() << qq.lastError();
            qDebug() << "Error on SELECT";
            timer->stop();
        }
        qDebug() << qq.lastError();
        qq.next();
        team = qq.value(0).toString();
        qDebug() << "Got the Team: " << team;
        qq.clear();
        qq.prepare("SELECT x FROM in_game WHERE UID=:uid");
        qq.bindValue(":uid",uid);
        if(!qq.exec())
        {
            qDebug() << qq.lastError();
            qDebug() << "Error on SELECT";
            timer->stop();
        }
        qq.next();
        x = qq.value(0).toInt();
        qDebug() << "Got the x: " << x;
        qq.clear();
        qq.prepare("SELECT y FROM in_game WHERE UID=:uid");
        qq.bindValue(":uid",uid);
        if(!qq.exec())
        {
            qDebug() << qq.lastError();
            qDebug() << "Error on SELECT";
            timer->stop();
        }
        qq.next();
        y = qq.value(0).toInt();
        qDebug() << "Got the y: " << y;
        qq.clear();
        qq.prepare("SELECT hasBall FROM in_game WHERE UID=:uid");
        qq.bindValue(":uid",uid);
        if(!qq.exec())
        {
            qDebug() << qq.lastError();
            qDebug() << "Error on SELECT";
            timer->stop();
        }
        qq.next();
        hasBall = qq.value(0).toInt();
        qDebug() << "Player hasBall" << hasBall;
        qq.clear();
//        qq.prepare("SELECT pixmaps FROM sprites WHERE team=:team AND hasBall=:hasBall");
//        qq.bindValue(":team",team);
//        qq.bindValue(":hasBall",hasBall);
//        if(!qq.exec())
//        {
//            qDebug() << qq.lastError();
//            qDebug() << "Error on SELECT";
//            timer->stop();
//        }
//        qq.next();
//        ba = qq.value(0).toByteArray();
//        qDebug() << "Byte Array " << ba;
//        qq.clear();

        // build packet and send  (packet string layout--> "PLAYER: uid team x y hasBall pixmap") spaces are how the client knows the difference
        msg.append("PLAYER: ");
        msg.append(QString::number(uid));
        msg.append(" ");
        msg.append(team);
        msg.append(" ");
        msg.append(QString::number(x));
        msg.append(" ");
        msg.append(QString::number(y));
        msg.append(" ");
        msg.append(QString::number(hasBall));
//        msg.append(" ");
//        msg.append(QString::fromUtf8(ba));
        qDebug() << "Message to be sent: " << msg;
        sendAll(msg);
    }

    qDebug() << "SERVER: Sent Data";
}
