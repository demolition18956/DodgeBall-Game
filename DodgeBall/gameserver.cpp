#include "gameserver.h"
#include <QTcpSocket>
#include <QByteArray>
#include <QPixmap>
#include <QFile>

//*************************************************************************************************//
//                                      Constructor                                                //
//*************************************************************************************************//

GameServer::GameServer(QObject* parent) :
    QTcpServer(parent)
{
    // Set maxmium amount of pending connections
    setMaxPendingConnections(6);

    // Listen and Process new connections
    connect(this, &QTcpServer::newConnection, this, &GameServer::ProcessNewConnections);

    // Open Port for Server at port 2224
    if(!this->listen(QHostAddress::Any,2224)){

        qDebug() << "SERVER: Could not start server";
    }
    else{

        qDebug() << "SERVER: Listening";
    }

    // Initialize Player Count
    playerCount = 0;

    // Configure SQL Server
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    // Checks validity of database
    if (!db.open()){

            qDebug() << db.lastError();
            qDebug() << "Error: Unable to connect due to above error";
    }

    // Otherwise, creates database
    QSqlQuery setup;

    // Create Players Table to record player info
    setup.exec("CREATE TABLE players(UID INT, playername TEXT, ready INT, ip TEXT, check(UID<=6))");

    // Checks validity of database
    if (!setup.isActive()){

        qDebug() << setup.lastError();
        qDebug() << "Creating Table 1 Error: unable to complete query";
        return;
    }

    // Create Sockets Table to number Players
    setup.exec("CREATE TABLE sockets(UID INT check(UID<=6))");

    // Check validity of database
    if (!setup.isActive()){

        qDebug() << setup.lastError();
        qDebug() << "Creating Table 2 Error: unable to complete query";
        return;
    }

    // Insert Values 1-6
    setup.exec("INSERT INTO sockets VALUES(1)");

    // Check validity of database
    if (!setup.isActive()){

        qDebug() << setup.lastError();
        qDebug() << "add 1 Error: unable to complete query";
        return;
    }

    setup.exec("INSERT INTO sockets VALUES(2)");

    // Check validity of database
    if (!setup.isActive()){

        qDebug() << setup.lastError();
        qDebug() << "add 2 Error: unable to complete query";
        return;
    }

    // Insert socket information into database
    setup.exec("INSERT INTO sockets VALUES(3)");
    setup.exec("INSERT INTO sockets VALUES(4)");
    setup.exec("INSERT INTO sockets VALUES(5)");
    setup.exec("INSERT INTO sockets VALUES(6)");

    // Check validity of database
    if (!setup.isActive()){

        qDebug() << setup.lastError();
        qDebug() << "Error: unable to complete query";
        return;
    }

    // Initialize timer
    timer = new QTimer(this);
}

//*************************************************************************************************//
//                                      Connections                                                //
//*************************************************************************************************//

// A potential Client has been detected
void GameServer::ProcessNewConnections(){

    qDebug() << "Signal Emitted";

    // Loop until all Connections are taken care of
    while(hasPendingConnections()){

        qDebug() << "SERVER: processing incoming connection";

        // If Lobby is already full
        if(playerCount >= 6){

            // Reject the player and return function
            QTcpSocket* sock = nextPendingConnection();

            sock->write("0");   // 0 for Rejected
            sock->abort();

            delete sock;

            qDebug() << "SERVER: player rejected!";
            return;
        }

        // Else,
        QTcpSocket* sock = this->nextPendingConnection();

        // Choose the last socket
        int sNum = getMinSocket();                                      // Grab the Socket offset from pointer
        sock->write(QByteArray::number(sNum+1) + QByteArray("\n"));     // Alert the player of their socket position (UID)

        QSqlQuery qqqq;                                                                     // Prepare to update insert Player
        qqqq.prepare("INSERT INTO players(UID, playername, ready, ip) VALUES(?, ?, ?, ?)"); // info into PLAYERS table
        qqqq.addBindValue(sNum+1);                                              // Bind UID
        qqqq.addBindValue("Player " + QString::number(++playerCount));          // Bind Default Name

        // Print current player count and name
        qDebug() << "Player Count " << playerCount;
        qDebug() << "Player " + QString::number(playerCount);

        int dfault = 0; // Set default to 0, not ready

        qqqq.addBindValue(dfault);                                              // Bind Default Ready Status (0 = false)
        qqqq.addBindValue(sock->peerAddress().toString());                       // Bind IP address (never used tho... oops)
        qDebug() << sock->peerAddress().toString();

        // Execute and Verify if command worked
        if (!qqqq.exec()){

           qDebug() << qqqq.lastError();
           qDebug() << "Error on INSERT";
        }

        playerSockets[sNum] = sock;                     // Set pointer to created socket
        this->UpdateClients();                          // Update all sockets on Player Info

        // Set up Reading Parsing Slot and Disconnection Slot
        connect(playerSockets[sNum], &QTcpSocket::readyRead,this, &GameServer::ReportReady);
        connect(playerSockets[sNum], &QTcpSocket::stateChanged, this, &GameServer::clientDisconnected);
    }
}

// Gets earliest available socket in array
int GameServer::getMinSocket(){

    // Create new sql query for MIN socket
    QSqlQuery q;

    // Test if there are any available sockets
    if (!q.exec("SELECT MIN(UID) FROM sockets WHERE UID NOT IN (SELECT UID FROM players)")){

        qDebug() << q.lastError();
        qDebug() << "Error on MIN";
    }

    // While there are available sockets, add to list
    if(q.next()){

        return (q.value(0).toInt() - 1);
    }

    return -1;
}

void GameServer::clientDisconnected(){

    // For statement relating to max number of players
    for(int i=0;i<6;i++){

        qDebug() << playerSockets[i];

        // Check if socket is not null
        if(playerSockets[i] != nullptr){

            qDebug() << playerSockets[i]->state();

            // Check if player is still connected
            if(playerSockets[i]->state() == QAbstractSocket::ConnectedState){
                // if so, continue
                continue;
            }
            // Else if playe is disconnected,
            else if (playerSockets[i]->state() == QAbstractSocket::UnconnectedState){

                // set socket to NULL (available) and decrement current player count
                playerSockets[i]->deleteLater();
                playerSockets[i] = nullptr;
                playerCount--;

                // SQL HERE
                QSqlQuery del;

                // If unable to delete player from database, throw error
                if (!del.exec("DELETE FROM players WHERE UID=" + QString::number(i+1))){

                    qDebug() << del.lastError();
                    qDebug() << "Error on DELETE";
                }

                // Update clients of new information
                this->UpdateClients();
                qDebug() << "Done";
            }
        }
    }
}

GameServer::~GameServer(){

}

//*************************************************************************************************//
//                                      Client Details                                             //
//*************************************************************************************************//

// Function for Updating Clients on mutual information in Lobby
void GameServer::UpdateClients(){

    // Create new sql query
    QSqlQuery socks;

    if (!socks.exec("SELECT UID FROM players")){

       qDebug() << socks.lastError();
       qDebug() << "Error on INSERT";

    }

    qDebug() << "Updating";

    //
    while(socks.next()){

        int playNum = 0;
        qDebug() << "Round";

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

        while (playNum < 6){

            out << "Number: " << QString::number(++playNum) << endl
                << "Player Name: No Player" << endl
                << "Ready: 0" << endl;
        }

        QSqlQuery ready("SELECT COUNT(ready) FROM players");

        if (ready.next()){

            out << "Joined: " << QString::number(ready.value(0).toInt()) << endl;
        }

        sock->write(block);
        qDebug() << block;

    }
}

//*************************************************************************************************//
//                                      Ready Up Information                                       //
//*************************************************************************************************//

void GameServer::ReportReady(){

    qDebug() << "Updating";
    QString str;
    int ind;

    for (int i = 0; i < 6; i++){

        qDebug() << i;

        if ((playerSockets[i] != nullptr) && playerSockets[i]->canReadLine()){

            qDebug() << "we're in";
            QTextStream in(playerSockets[i]);

            while (in.readLineInto(&str)){

                qDebug() << str;

                if ((ind = str.indexOf(" Ready")) != -1){

                    qDebug() << str.left(1);
                    QSqlQuery qq;

                    if (!qq.exec("UPDATE players SET ready = 1 WHERE UID=" + str.left(1))){

                        qDebug() << qq.lastError();
                        qDebug() << "Error on UPDATE";
                    }

                    if (!qq.exec("SELECT COUNT(UID) FROM players WHERE ready=1")){

                        qDebug() << qq.lastError();
                        qDebug() << "Error on SELECT";
                    }

                    qq.next();
                    qDebug() << qq.value(0).toInt();

                    if((qq.value(0).toInt() == playerCount) && playerCount >= 1){

                        qDebug() << "SERVER: Starting Game " << qq.value(0).toInt();
                        StartGame();
                    }
                }

                else if ((ind = str.indexOf(" NotReady")) != -1){

                    qDebug() << str.left(1);
                    QSqlQuery qq;

                    if (!qq.exec("UPDATE players SET ready = 0 WHERE UID=" + str.left(1))){

                        qDebug() << qq.lastError();
                        qDebug() << "Error on UPDATE";

                     }
                }

                else if ((ind = str.indexOf(" Name ")) != -1){

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

                else if ((ind = str.indexOf("Player: ")) != -1){
                    qDebug() << "WE are in";
                    QString buffer;
                    QTextStream _in(&str, QIODevice::ReadOnly);
                    _in >> buffer;
                    int x;
                    int y;
                    bool hasBall;

                    buffer.clear();
                    _in >> buffer;  // x read
                    x = buffer.toInt();
                    buffer.clear();
                    _in >> buffer;  // y read
                    y = buffer.toInt();
                    buffer.clear();
                    _in >> buffer;   // hasBall read
                    hasBall = buffer.toInt();


                    qDebug() << "Player Data Read: ";
                    qDebug() << "x: " << x;
                    qDebug() << "y: " << y;
                    qDebug() << "hasBall: " << hasBall;

                    QSqlQuery q;
                    q.prepare("UPDATE in_game SET x=:X, y=:Y, hasBall=:HasBall WHERE UID=:uid");
                    q.bindValue(":X",x);
                    q.bindValue(":Y",y);
                    q.bindValue(":HasBall",hasBall);
                    q.bindValue(":uid",i+1);

                    if(!q.exec())
                    {
                        qDebug() << q.lastError();
                        qDebug() << "Error on UPDATE";
                    }

                    break;
                }

            this->UpdateReady();
            break;
            }
        }
    }
}

void GameServer::UpdateReady(){

    QSqlQuery socks;

    if (!socks.exec("SELECT UID FROM players")){

       qDebug() << socks.lastError();
       qDebug() << "Error on INSERT";

    }

    while(socks.next()){

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

//*************************************************************************************************//
//                                      Server Details                                             //
//*************************************************************************************************//

// send a message to start the game
void GameServer::StartGame(){

    qDebug() << "SERVER: Starting Game";
    this->sendAll("start");
    QSqlQuery start;

    // table for data associated with active players
    if(!start.exec("CREATE TABLE in_game(UID INT, x INT, y INT, hasBall INT, team TEXT)")){

        qDebug() << start.lastError();
        qDebug() << "Error on CREATE";
    }

    qDebug() << "In-game table created";
    start.clear();

    QSqlQuery q;

    // Populate in_game table
    if(!q.exec("SELECT UID FROM players")){

        qDebug() << q.lastError();
        qDebug() << "Error on SELECT";
    }

    QString team = "red";

    int plays = playerCount/2 + (playerCount % 2);
    qDebug() << plays;

    qreal off = -YMAX / 2*(((qreal)plays -1)/plays);
    qDebug() << off;

    while(q.next()){

        qreal x;

        x = (team == "red") ? -XMAX/2 + 100 : XMAX/2 - 100;

        qDebug() << "X = " << x;
        qDebug() << "Offset = " << off;

        QSqlQuery qq;

        qq.prepare("INSERT INTO in_game VALUES(:uid, :x, :y, :hasBall, :team)");

        qq.bindValue(":uid", q.value(0).toInt());
        qq.bindValue(":x", (int)x);
        qq.bindValue(":y", (int)off);
        qq.bindValue(":hasBall", 0);
        qq.bindValue(":team", team);

        if(!qq.exec()){

            qDebug() << start.lastError();
            qDebug() << "Error on INSERT";
        }

        qq.clear();

        if(team == "red"){

            team = "blue";
        }

        else{

            off += YMAX/plays;
            team = "red";
        }
    }
    q.clear();

    // table for data associated with active dodgeballs (flying and stationary)
    if(!start.exec("CREATE TABLE dodgeballs(bid INT, x INT, y INT, isFlying INT)")){

        qDebug() << start.lastError();
        qDebug() << "Error on CREATE";
    }

    qDebug() << "dodgeballs table created";
    start.clear();

    timer->start(250);
    connect(timer, &QTimer::timeout, this, &GameServer::onTimeout);
    qDebug() << "SERVER: Timer Started!";
}

//function that sends a message to everybody
void GameServer::sendAll(QString message){

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

void GameServer::onTimeout(){

    QSqlQuery q;

    if(!q.exec("SELECT UID FROM in_game")){

        qDebug() << q.lastError();
        qDebug() << "Error on SELECT";
        timer->stop();
    }

    while(q.next()){

        QSqlQuery qq;
        QString msg;
        QString team;

        int x;
        int y;
        bool hasBall;
        int uid;

        uid = q.value(0).toInt();
        qDebug() << "Got the UID: " << uid;

        qq.prepare("SELECT team, x, y, hasBall FROM in_game WHERE UID=:uid");
        qq.bindValue(":uid",uid);

        if(!qq.exec()){

            qDebug() << qq.lastError();
            qDebug() << "Error on SELECT";
            timer->stop();
        }

        if (!qq.next()){

            qDebug() << qq.lastError();
            qDebug() << "Error on SELECT";
            timer->stop();
        }

        team = qq.value(0).toString();
        qDebug() << "Got the Team: " << team;

        x = qq.value(1).toInt();
        qDebug() << "Got the x: " << x;

        y = qq.value(2).toInt();
        qDebug() << "Got the y: " << y;

        hasBall = qq.value(3).toInt();
        qDebug() << "Player hasBall" << hasBall;

        qq.clear();

        // build packet and send  (packet string layout--> "PLAYER: uid team x y hasBall") spaces are how the client knows the difference
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

        qDebug() << "Message to be sent: " << msg;
//        disconnect(playerSockets[uid], SIGNAL(readyRead()),this, SLOT(ReportReady()));
//        connect(playerSockets[uid], SIGNAL(readyRead()),this, SLOT(ReportMovement()));
        sendAll(msg);
    }

    qDebug() << "SERVER: Sent Data";
}

void GameServer::ReportMovement(){

    qDebug() << "Updating";
    QString str;
    int ind;

    for (int i = 0; i < 6; i++){
        if ((playerSockets[i] != nullptr) && playerSockets[i]->canReadLine()){

            qDebug() << "we're in";
            QTextStream in(playerSockets[i]);

            while (in.readLineInto(&str)){

                qDebug() << str;

                if ((ind = str.indexOf("Player: ")) != -1){
                    qDebug() << "WE are in";
                    QString buffer;
                    QTextStream _in(&str, QIODevice::ReadOnly);
                    _in >> buffer;
                    int x;
                    int y;
                    bool hasBall;

                    buffer.clear();
                    _in >> buffer;  // x read
                    x = buffer.toInt();
                    buffer.clear();
                    _in >> buffer;  // y read
                    y = buffer.toInt();
                    buffer.clear();
                    _in >> buffer;   // hasBall read
                    hasBall = buffer.toInt();


                    qDebug() << "Player Data Read: ";
                    qDebug() << "x: " << x;
                    qDebug() << "y: " << y;
                    qDebug() << "hasBall: " << hasBall;

                    QSqlQuery q;
                    q.prepare("UPDATE in_game SET x=:X, y=:Y, hasBall=:HasBall WHERE UID=:uid");
                    q.bindValue(":X",x);
                    q.bindValue(":Y",y);
                    q.bindValue(":HasBall",hasBall);
                    q.bindValue(":uid",i+1);

                    if(!q.exec())
                    {
                        qDebug() << q.lastError();
                        qDebug() << "Error on UPDATE";
                    }

                    break;
                }

            }
        }
    }
}
