#include "gameserver.h"
#include <QTcpSocket>
#include <QByteArray>
#include <QPixmap>
#include <QFile>
#include <QNetworkProxy>

//*************************************************************************************************//
//                                      Constructor                                                //
//*************************************************************************************************//

GameServer::GameServer(int portNum, QObject* parent) :
    QTcpServer(parent)
{
    // Set maxmium amount of pending connections
    setMaxPendingConnections(6);

    // Listen and Process new connections
    connect(this, &QTcpServer::newConnection, this, &GameServer::ProcessNewConnections);

    // Open Port for Server at designated port
    if(!this->listen(QHostAddress::Any,portNum)){

        //qDebug() << "SERVER: Could not start server";
    }
    else{

        //qDebug() << "SERVER: Listening";
    }

    // Initialize Player Count
    playerCount = 0;
    inLobby = true;

    // Configure SQL Server
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");

    // Checks validity of database
    if (!db.open()){

            //qDebug() << db.lastError();
            //qDebug() << "Error: Unable to connect due to above error";
    }

    // Otherwise, creates database
    QSqlQuery setup;

    // Create Players Table to record player info
    setup.exec("CREATE TABLE players(UID INT, playername TEXT, ready INT, ip TEXT, check(UID<=6))");

    // Checks validity of database
    if (!setup.isActive()){

        //qDebug() << setup.lastError();
        //qDebug() << "Creating Table 1 Error: unable to complete query";
        return;
    }

    // Create Sockets Table to number Players
    setup.exec("CREATE TABLE sockets(UID INT check(UID<=6))");

    // Check validity of database
    if (!setup.isActive()){

        //qDebug() << setup.lastError();
        //qDebug() << "Creating Table 2 Error: unable to complete query";
        return;
    }

    // Insert Values 1-6
    setup.exec("INSERT INTO sockets VALUES(1)");

    // Check validity of database
    if (!setup.isActive()){

        //qDebug() << setup.lastError();
        //qDebug() << "add 1 Error: unable to complete query";
        return;
    }

    setup.exec("INSERT INTO sockets VALUES(2)");

    // Check validity of database
    if (!setup.isActive()){

        //qDebug() << setup.lastError();
        //qDebug() << "add 2 Error: unable to complete query";
        return;
    }

    // Insert socket information into database
    setup.exec("INSERT INTO sockets VALUES(3)");
    setup.exec("INSERT INTO sockets VALUES(4)");
    setup.exec("INSERT INTO sockets VALUES(5)");
    setup.exec("INSERT INTO sockets VALUES(6)");

    // Check validity of database
    if (!setup.isActive()){

        //qDebug() << setup.lastError();
        //qDebug() << "Error: unable to complete query";
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

    //qDebug() << "Signal Emitted";

    // Loop until all Connections are taken care of
    while(hasPendingConnections()){

        //qDebug() << "SERVER: processing incoming connection";

        // If Lobby is already full
        if(!inLobby || playerCount >= 6){

            // Reject the player and return function
            QTcpSocket* sock = nextPendingConnection();
            sock->setProxy(QNetworkProxy::NoProxy);

            sock->write("0");   // 0 for Rejected
            sock->abort();

            delete sock;

            //qDebug() << "SERVER: player rejected!";
            return;
        }

        // Else,
        QTcpSocket* sock = this->nextPendingConnection();
        sock->setProxy(QNetworkProxy::NoProxy);

        // Choose the last socket
        int sNum = getMinSocket();                                      // Grab the Socket offset from pointer
        sock->write(QByteArray::number(sNum+1) + QByteArray("\n"));     // Alert the player of their socket position (UID)

        QSqlQuery qqqq;                                                                     // Prepare to update insert Player
        qqqq.prepare("INSERT INTO players(UID, playername, ready, ip) VALUES(?, ?, ?, ?)"); // info into PLAYERS table
        qqqq.addBindValue(sNum+1);                                              // Bind UID
        qqqq.addBindValue("Player " + QString::number(++playerCount));          // Bind Default Name

        // Print current player count and name
        //qDebug() << "Player Count " << playerCount;
        //qDebug() << "Player " + QString::number(playerCount);

        int dfault = 0; // Set default to 0, not ready

        qqqq.addBindValue(dfault);                                              // Bind Default Ready Status (0 = false)
        qqqq.addBindValue(sock->peerAddress().toString());                       // Bind IP address (never used tho... oops)
        //qDebug() << sock->peerAddress().toString();

        // Execute and Verify if command worked
        if (!qqqq.exec()){

           //qDebug() << qqqq.lastError();
           //qDebug() << "Error on INSERT";
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

        //qDebug() << q.lastError();
        //qDebug() << "Error on MIN";
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

        //qDebug() << playerSockets[i];

        // Check if socket is not null
        if(playerSockets[i] != nullptr){

            //qDebug() << playerSockets[i]->state();

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

                // If the Game is in Play
                if (!inLobby){
                    QString msg;
                    QSqlQuery q;
                    // Delete Player from game stats
                    if (!del.exec("DELETE FROM in_game WHERE UID=" + QString::number(i+1))){

                        //qDebug() << del.lastError();
                        //qDebug() << "Error on DELETE";
                    }
                    // Get Clients to delete Player icon
                    sendAll("HIT: " + QString::number(i+1));

                    // Test for if red team is no longer remaining
                    q.prepare("SELECT COUNT(team) FROM in_game WHERE team=:Team");
                    q.bindValue(":Team", "red");

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on SELECT";
                    }
                    q.next();

                    // If no more red team, Blue team wins
                    if(q.value(0).toInt() == 0)
                    {
                        q.finish();
                        qDebug() << "Only 1 Team Remains!!!";

                        qDebug() << "BLUE WINS!!!";

                        msg.clear();
                        msg = "Finish: blue";
                        this->sendAll(msg);
                        timer->stop();
                        inLobby = true;
                        QSqlQuery qq;
                        UpdateReady();
                        qq.prepare("DROP TABLE in_game");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.clear();
                        qq.prepare("DROP TABLE dodgeballs");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.finish();
                    }
                    q.clear();

                    // Test for if blue team is no longer remaining
                    q.prepare("SELECT COUNT(team) FROM in_game WHERE team=:Team");
                    q.bindValue(":Team", "blue");

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on SELECT";
                    }
                    q.next();

                    // If no more blue team, Red team wins
                    if(q.value(0).toInt() == 0)
                    {
                        q.finish();
                        qDebug() << "Only 1 Team Remains!!!";

                        qDebug() << "RED WINS!!!";

                        msg.clear();
                        msg = "Finish: red";
                        this->sendAll(msg);
                        timer->stop();
                        inLobby = true;
                        QSqlQuery qq;
                        UpdateReady();
                        qq.prepare("DROP TABLE in_game");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.clear();
                        qq.prepare("DROP TABLE dodgeballs");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.finish();
                    }
                }

                // Update clients of new information
                this->UpdateClients();
                //qDebug() << "Done";
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

       //qDebug() << socks.lastError();
       //qDebug() << "Error on INSERT";

    }

    //qDebug() << "Updating";


    while(socks.next()){

        int playNum = 0;
        //qDebug() << "Round";

        QTcpSocket* sock = playerSockets[socks.value(0).toInt()-1];
        //qDebug() << sock;

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
        //qDebug() << block;

    }
}

//*************************************************************************************************//
//                                      Ready Up Information                                       //
//*************************************************************************************************//

// Handles information from all clients
void GameServer::ReportReady(){

    //qDebug() << "Updating";
    QString str;
    int ind;

    for (int i = 0; i < 6; i++){

        //qDebug() << i;

        if ((playerSockets[i] != nullptr) && playerSockets[i]->canReadLine()){

            //qDebug() << "we're in";
            QTextStream in(playerSockets[i]);

            while (in.readLineInto(&str)){

                //qDebug() << str;

                if ((ind = str.indexOf(" Ready")) != -1){

                    //qDebug() << str.left(1);
                    QSqlQuery qq;

                    if (!qq.exec("UPDATE players SET ready = 1 WHERE UID=" + str.left(1))){

                        //qDebug() << qq.lastError();
                        //qDebug() << "Error on UPDATE";
                    }

                    if (!qq.exec("SELECT COUNT(UID) FROM players WHERE ready=1")){

                        //qDebug() << qq.lastError();
                        //qDebug() << "Error on SELECT";
                    }

                    qq.next();
                    //qDebug() << qq.value(0).toInt();

                    if((qq.value(0).toInt() == playerCount) && playerCount > 1) // Starts the game when 2 or more players are readied up
                    {

                        //qDebug() << "SERVER: Starting Game " << qq.value(0).toInt();
                        UpdateReady();
                        inLobby = false;
                        StartGame();
                    }
                }

                else if ((ind = str.indexOf(" NotReady")) != -1) // Update when players are not ready
                {

                    //qDebug() << str.left(1);
                    QSqlQuery qq;

                    if (!qq.exec("UPDATE players SET ready = 0 WHERE UID=" + str.left(1))){

                        //qDebug() << qq.lastError();
                        //qDebug() << "Error on UPDATE";

                     }
                }

                else if ((ind = str.indexOf(" Name ")) != -1) // Set player nickname
                {

                    ind += 6;
                    QSqlQuery qq;

                    if (!qq.exec("UPDATE players SET playername = \""
                                 + str.right(str.length() - ind)
                                 + "\" WHERE UID="+str.left(1)))
                    {

                        //qDebug() << qq.lastError();
                        //qDebug() << "Error on UPDATE";
                    }
                }

                else if ((ind = str.indexOf("Player: ")) != -1) // Updates coordinates of other players and if they are holding a ball
                {
                    //qDebug() << "WE are in";
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


                    //qDebug() << "Player Data Read: ";
                    //qDebug() << "x: " << x;
                    //qDebug() << "y: " << y;
                    //qDebug() << "hasBall: " << hasBall;

                    QSqlQuery q;
                    q.prepare("UPDATE in_game SET x=:X, y=:Y, hasBall=:HasBall WHERE UID=:uid");
                    q.bindValue(":X",x);
                    q.bindValue(":Y",y);
                    q.bindValue(":HasBall",hasBall);
                    q.bindValue(":uid",i+1);

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on UPDATE";
                    }

                }

                else if ((ind = str.indexOf("Throw: ")) != -1) // Updates ball position
                {

                    QString buffer;
                    QTextStream _in(&str, QIODevice::ReadOnly);
                    _in >> buffer;
                    int x;
                    int y;
                    int bid;
                    QString team;

                    buffer.clear();
                    _in >> buffer;  // bid read
                    bid = buffer.toInt();
                    buffer.clear();
                    _in >> buffer;  // x read
                    x = buffer.toInt();
                    buffer.clear();
                    _in >> buffer;   // y read
                    y = buffer.toInt();
                    buffer.clear();
                    _in >> buffer;   // team read
                    team = buffer;

                    //qDebug() << "Ball Throw Read: ";
                    //qDebug() << "bid: " << bid;
                    //qDebug() << "x: " << x;
                    //qDebug() << "y: " << y;
                    //qDebug() << "team: " << team;

                    QSqlQuery q;
                    q.prepare("UPDATE dodgeballs SET x=:X, y=:Y, isHeld=:IsHeld, team=:Team WHERE bid=:BID");
                    q.bindValue(":BID",bid);
                    q.bindValue(":X",x);
                    q.bindValue(":Y",y);
                    q.bindValue(":IsHeld",0);
                    q.bindValue(":Team",team);

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on UPDATE";
                    }
                }

                else if ((ind = str.indexOf("Grab: ")) != -1) // Updates if a player grabs a ball
                {

                    QString buffer;
                    QTextStream _in(&str, QIODevice::ReadOnly);
                    _in >> buffer;
                    int uid = i+1;
                    int bid;

                    buffer.clear();
                    _in >> buffer;  // bid read
                    bid = buffer.toInt();

                    //qDebug() << "Ball Grab Read: ";
                    //qDebug() << "bid: " << bid;

                    QSqlQuery q;
                    q.prepare("UPDATE dodgeballs SET isHeld=:IsHeld WHERE bid=:BID");
                    q.bindValue(":BID",bid);
                    q.bindValue(":IsHeld",1);

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on UPDATE";
                    }

                    q.prepare("UPDATE in_game SET hasBall=:HasBall WHERE UID=:uid");
                    q.bindValue(":HasBall",1);
                    q.bindValue(":uid",uid);

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on UPDATE";
                    }
                }

                else if ((ind = str.indexOf("Hit: ")) != -1)
                {   // Updates player information upon a successful hit, also updates information for winning team

                    QString buffer;
                    QTextStream _in(&str, QIODevice::ReadOnly);
                    _in >> buffer;

                    buffer.clear();
                    _in >> buffer;  // uid read
                    int uid = buffer.toInt();

                    qDebug() << "SERVER: Hit Read";
                    qDebug() << "uid: " << uid;

                    QSqlQuery q;
                    q.prepare("DELETE FROM in_game WHERE UID=:uid");
                    q.bindValue(":uid",uid);

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on DELETE";
                    }

                    QString msg = "Hit: ";
                    msg.append(QString::number(uid));

                    this->sendAll(msg);
                    qDebug() << "CHECKING FOR WINNER";
                    q.prepare("SELECT COUNT(team) FROM in_game WHERE team=:Team");
                    q.bindValue(":Team", "red");

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on SELECT";
                    }
                    q.next();
                    if(q.value(0).toInt() == 0)
                    {
                        q.finish();
                        qDebug() << "Only 1 Team Remains!!!";

                        qDebug() << "BLUE WINS!!!";

                        msg.clear();
                        msg = "Finish: blue";
                        this->sendAll(msg);
                        timer->stop();
                        inLobby = true;
                        QSqlQuery qq;
                        UpdateReady();
                        qq.prepare("DROP TABLE in_game");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.clear();
                        qq.prepare("DROP TABLE dodgeballs");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.finish();
                        return;
                    }
                    q.clear();
                    q.prepare("SELECT COUNT(team) FROM in_game WHERE team=:Team");
                    q.bindValue(":Team", "blue");

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on SELECT";
                    }
                    q.next();
                    if(q.value(0).toInt() == 0)
                    {
                        q.finish();
                        qDebug() << "Only 1 Team Remains!!!";

                        qDebug() << "RED WINS!!!";

                        msg.clear();
                        msg = "Finish: red";
                        this->sendAll(msg);
                        timer->stop();
                        inLobby = true;
                        QSqlQuery qq;
                        UpdateReady();
                        qq.prepare("DROP TABLE in_game");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.clear();
                        qq.prepare("DROP TABLE dodgeballs");
                        if(!qq.exec())
                        {
                            qDebug() << q.lastError();
                            qDebug() << "Error on DROP";
                        }
                        qq.finish();
                        return;
                    }
                }

                else if ((ind = str.indexOf("Drop: ")) != -1) // Updates if a player grabs a ball
                {

                    QString buffer;
                    QTextStream _in(&str, QIODevice::ReadOnly);
                    _in >> buffer;
                    int x;
                    int y;
                    int bid;

                    buffer.clear();
                    _in >> buffer;  // bid read
                    bid = buffer.toInt();

                    buffer.clear();
                    _in >> buffer;  // bid read
                    x = buffer.toInt();

                    buffer.clear();
                    _in >> buffer;  // bid read
                    y = buffer.toInt();

                    //qDebug() << "Ball Grab Read: ";
                    //qDebug() << "bid: " << bid;

                    QSqlQuery q;
                    q.prepare("UPDATE dodgeballs SET isHeld=:IsHeld, x=:X, y=:Y, team=:TEAM WHERE bid=:BID");
                    q.bindValue(":BID",bid);
                    q.bindValue(":IsHeld",0);
                    q.bindValue(":X",x);
                    q.bindValue(":Y",y);
                    q.bindValue(":TEAM","noteam");

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on UPDATE";
                    }

                }

                if (inLobby){
                    this->UpdateReady();
                    break;
                }

            }
        }
    }
}

void GameServer::UpdateReady() // Sends ready information to other players
{

    QSqlQuery socks;

    if (!socks.exec("SELECT UID FROM players")){

       //qDebug() << socks.lastError();
       //qDebug() << "Error on INSERT";

    }

    while(socks.next()){

        int playNum = 0;

        QTcpSocket* sock = playerSockets[socks.value(0).toInt()-1];
        //qDebug() << sock;

        QByteArray block;
        QTextStream out(&block, QIODevice::WriteOnly);
        QSqlQuery info("SELECT playername, ready FROM players");

        while (info.next()){
            out << "Number: " << QString::number(++playNum) << endl
                << "Player Name: " << info.value(0).toString() << endl
                << "Ready: " << QString::number(info.value(1).toInt()) << endl;
        }

        sock->write(block);
        //qDebug() << block;


    }
}

//*************************************************************************************************//
//                                      Server Details                                             //
//*************************************************************************************************//

// sends a message to start the game
void GameServer::StartGame(){

    //qDebug() << "SERVER: Starting Game";
    this->sendAll("start");
    QSqlQuery start;

    // table for data associated with active players
    if(!start.exec("CREATE TABLE in_game(UID INT, x INT, y INT, hasBall INT, team TEXT)")){

        qDebug() << start.lastError();
        qDebug() << "Error on CREATE";
    }

    //qDebug() << "In-game table created";
    start.clear();

    QSqlQuery q;

    // Populate in_game table
    if(!q.exec("SELECT UID FROM players")){

        //qDebug() << q.lastError();
        //qDebug() << "Error on SELECT";
    }

    QString team = "red";

    int plays = playerCount/2 + (playerCount % 2);
    //qDebug() << plays;

    qreal off = -YMAX / 2*(((qreal)plays -1)/plays);
    //qDebug() << off;

    while(q.next()){

        qreal x;

        x = (team == "red") ? -XMAX/2 + 100 : XMAX/2 - 100;

        //qDebug() << "X = " << x;
        //qDebug() << "Offset = " << off;

        QSqlQuery qq;

        qq.prepare("INSERT INTO in_game VALUES(:uid, :x, :y, :hasBall, :team)");

        qq.bindValue(":uid", q.value(0).toInt());
        qq.bindValue(":x", (int)x);
        qq.bindValue(":y", (int)off);
        qq.bindValue(":hasBall", 0);
        qq.bindValue(":team", team);

        if(!qq.exec()){

            //qDebug() << start.lastError();
            //qDebug() << "Error on INSERT";
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
    if(!start.exec("CREATE TABLE dodgeballs(bid INT, x INT, y INT, isHeld INT, team TEXT)")){

        //qDebug() << start.lastError();
        //qDebug() << "Error on CREATE";
    }

    //qDebug() << "dodgeballs table created";
    start.clear();

    // populate the dodgeballs table (spawn in first set of dodgeballs)
    // newly spawned dogdeballs will sit on the middle line
    for(int i=1;i<=4;i++)
    {
        q.prepare("INSERT INTO dodgeballs VALUES(:bid, :x, :y, :isHeld, :team)");
        q.bindValue(":bid", i);
        q.bindValue(":x", 0);
        q.bindValue(":y", YMAX/2-(180*(i-1))-96);
        q.bindValue(":isHeld", 0);
        q.bindValue(":team", "noteam");  // team will be assigned "red" or "blue" once picked up to determine the direction it will be thrown
        if(!q.exec()){

            //qDebug() << q.lastError();
            //qDebug() << "Error on INSERT";
        }
        q.clear();
    }

    timer->start(30);
    connect(timer, &QTimer::timeout, this, &GameServer::onTimeout);
    //qDebug() << "SERVER: Timer Started!";
}

//function that sends a message to everybody
void GameServer::sendAll(QString message)
{

        //qDebug() << "Sending message to everybody";

       for(int i = 0; i < 6; i++){
            if((playerSockets[i] != nullptr) && (playerSockets[i]->state() == QAbstractSocket::ConnectedState)){

                //qDebug() << playerSockets[i];

                QByteArray block;
                QTextStream out(&block, QIODevice::ReadWrite);

                out << message << endl;

                playerSockets[i]->write(block);
                playerSockets[i]->flush();
            }

        }
}

void GameServer::onTimeout() // Sends all gameplay data to other players
{

    QSqlQuery q;

    if(!q.exec("SELECT UID FROM in_game")){

        //qDebug() << q.lastError();
        //qDebug() << "Error on SELECT";
        timer->stop();
    }

    // send player data
    while(q.next()){

        QSqlQuery qq;
        QString msg;
        QString team;

        int x;
        int y;
        bool hasBall;
        int uid;

        uid = q.value(0).toInt();
        //qDebug() << "Got the UID: " << uid;

        qq.prepare("SELECT team, x, y, hasBall FROM in_game WHERE UID=:uid");
        qq.bindValue(":uid",uid);

        if(!qq.exec()){

            //qDebug() << qq.lastError();
            //qDebug() << "Error on SELECT";
            timer->stop();
        }

        if (!qq.next()){

            //qDebug() << qq.lastError();
            //qDebug() << "Error on SELECT";
            timer->stop();
        }

        team = qq.value(0).toString();
        //qDebug() << "Got the Team: " << team;

        x = qq.value(1).toInt();
        //qDebug() << "Got the x: " << x;

        y = qq.value(2).toInt();
        //qDebug() << "Got the y: " << y;

        hasBall = qq.value(3).toInt();
        //qDebug() << "Player hasBall" << hasBall;

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

        //qDebug() << "Message to be sent: " << msg;
        sendAll(msg);
    }
    q.clear();

    if(!q.exec("SELECT bid FROM dodgeballs")){

        //qDebug() << q.lastError();
        //qDebug() << "Error on SELECT";
        timer->stop();
    }

    // send dodgeball data
    while(q.next())
    {

        QSqlQuery qq;
        QString msg;

        int x;
        int y;
        bool isHeld;
        int bid;
        QString team;

        bid = q.value(0).toInt();
        //qDebug() << "Got the bid: " << bid;

        qq.prepare("SELECT x, y, isHeld, team FROM dodgeballs WHERE bid=:bid");
        qq.bindValue(":bid",bid);

        if(!qq.exec()){

            //qDebug() << qq.lastError();
            //qDebug() << "Error on SELECT";
            timer->stop();
        }

        if (!qq.next()){

            //qDebug() << qq.lastError();
            //qDebug() << "Error on NEXT";
            timer->stop();
        }

        x = qq.value(0).toInt();
        //qDebug() << "Got the x: " << x;

        y = qq.value(1).toInt();
        //qDebug() << "Got the y: " << y;

        isHeld = qq.value(2).toInt();
        //qDebug() << "Got the isHeld: " << isHeld;

        team = qq.value(3).toString();
        //qDebug() << "Got the team: " << team;

        qq.clear();

        // build packet and send  (packet string layout--> "BALL: bid x y isHeld team") spaces are how the client knows the difference
        msg.append("BALL: ");
        msg.append(QString::number(bid));
        msg.append(" ");
        msg.append(QString::number(x));
        msg.append(" ");
        msg.append(QString::number(y));
        msg.append(" ");
        msg.append(QString::number(isHeld));
        msg.append(" ");
        msg.append(team);

        //qDebug() << "Message to be sent: " << msg;
        sendAll(msg);
    }

    //qDebug() << "SERVER: Sent Data";
}

void GameServer::ReportMovement() // Report when a player moves
{

    //qDebug() << "Updating";
    QString str;
    int ind;

    for (int i = 0; i < 6; i++){
        if ((playerSockets[i] != nullptr) && playerSockets[i]->canReadLine()){

            //qDebug() << "we're in";
            QTextStream in(playerSockets[i]);

            while (in.readLineInto(&str)){

                //qDebug() << str;

                if ((ind = str.indexOf("Player: ")) != -1){
                    //qDebug() << "WE are in";
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


                    //qDebug() << "Player Data Read: ";
                    //qDebug() << "x: " << x;
                    //qDebug() << "y: " << y;
                    //qDebug() << "hasBall: " << hasBall;

                    QSqlQuery q;
                    q.prepare("UPDATE in_game SET x=:X, y=:Y, hasBall=:HasBall WHERE UID=:uid");
                    q.bindValue(":X",x);
                    q.bindValue(":Y",y);
                    q.bindValue(":HasBall",hasBall);
                    q.bindValue(":uid",i+1);

                    if(!q.exec())
                    {
                        //qDebug() << q.lastError();
                        //qDebug() << "Error on UPDATE";
                    }

                    break;
                }

            }
        }
    }
}
