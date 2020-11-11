#include "gameserver.h"
#include <QTcpSocket>
#include <QByteArray>

GameServer::GameServer(QObject* parent) :
    QTcpServer(parent)
{
    setMaxPendingConnections(6);
    connect(this, &QTcpServer::newConnection, this, &GameServer::ProcessNewConnections);
    if(!this->listen(QHostAddress::Any,2224)){
        qDebug() << "SERVER: Could not start server";
    }
    else{
        qDebug() << "SERVER: Listening";
    }

    playerCount = 0;
    for(int i=0;i<6;i++)
    {
        players[i] = new Player();
    }
    db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(":memory:");
    if (!db.open()) {
            qDebug() << db.lastError();
            qDebug() << "Error: Unable to connect due to above error";
    }
    QSqlQuery setup;

    setup.exec("CREATE TABLE players(UID INT, playername TEXT, ready INT, ip TEXT, check(UID<=6))");
    if (!setup.isActive()) {
        qDebug() << setup.lastError();
        qDebug() << "Creating Table 1 Error: unable to complete query";
        return;
    }
    setup.exec("CREATE TABLE sockets(UID INT check(UID<=6))");
    if (!setup.isActive()) {
        qDebug() << setup.lastError();
        qDebug() << "Creating Table 2 Error: unable to complete query";
        return;
    }
    setup.exec("INSERT INTO sockets VALUES(1)");
    if (!setup.isActive()) {
        qDebug() << setup.lastError();
        qDebug() << "add 1 Error: unable to complete query";
        return;
    }
    setup.exec("INSERT INTO sockets VALUES(2)");
    if (!setup.isActive()) {
        qDebug() << setup.lastError();
        qDebug() << "add 2 Error: unable to complete query";
        return;
    }
    setup.exec("INSERT INTO sockets VALUES(3)");
    setup.exec("INSERT INTO sockets VALUES(4)");
    setup.exec("INSERT INTO sockets VALUES(5)");
    setup.exec("INSERT INTO sockets VALUES(6)");

    if (!setup.isActive()) {
        qDebug() << setup.lastError();
        qDebug() << "Error: unable to complete query";
        return;
    }

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

        sock->write(QByteArray::number(++playerCount) + QByteArray("\n"));
        QSqlQuery qqqq;
        int sNum = getMinSocket();
        qqqq.prepare("INSERT INTO players(UID, playername, ready, ip) VALUES(?, ?, ?, ?)");
        qqqq.addBindValue(playerCount);
        qDebug() << "Player Count " << playerCount;
        qqqq.addBindValue("Player " + QString::number(playerCount));
        qDebug() << "Player " + QString::number(playerCount);
        int dfault = 0;
        qqqq.addBindValue(dfault);
        qqqq.addBindValue(sock->peerAddress().toString());
        qDebug() << sock->peerAddress().toString();
        if (!qqqq.exec()) {
           qDebug() << qqqq.lastError();
           qDebug() << "Error on INSERT";
        }
        playerSockets[sNum] = sock;
        this->UpdateClients();
        connect(playerSockets[sNum], &QTcpSocket::readyRead,this, &GameServer::ReportReady);
    }
}

GameServer::~GameServer()
{
    for(int i=0;i<6;i++)
    {
        delete players[i];
    }
}

void GameServer::UpdateClients() {

    QSqlQuery socks;
    if (!socks.exec("SELECT UID FROM players")) {
       qDebug() << socks.lastError();
       qDebug() << "Error on INSERT";

    }
    qDebug() << "Updating";
    while(socks.next()){
        qDebug() << "Round";
        QTcpSocket* sock = playerSockets[socks.value(0).toInt()-1];
        qDebug() << sock;
        QByteArray block;
        QTextStream out(&block, QIODevice::WriteOnly);
        QSqlQuery info("SELECT UID, playername, ready FROM players");
        while (info.next()){
            out << "Number: " << QString::number(info.value(0).toInt()) << endl
                << "Player Name: " << info.value(1).toString() << endl
                << "Ready: " << QString::number(info.value(2).toInt()) << endl;
        }
        QSqlQuery ready("SELECT COUNT(ready) FROM players");
        if (ready.next()){
            out << "Joined: " << QString::number(ready.value(0).toInt()) << endl;
        }
//        sock = players[i]->socket;
        sock->write(block);
        qDebug() << block;

    }
}

void GameServer::playerJoined()
{
    for(int i = 0; i < 6; i++)
    {
        if(players[i]->socket->state() == QAbstractSocket::ConnectedState)
        {
            players[i]->socket->write("2");
            players[i]->socket->flush();
        }
    }

}

int GameServer::getMinSocket()
{
    QSqlQuery q;
    if (!q.exec("SELECT MIN(UID) FROM sockets WHERE UID NOT IN (SELECT UID FROM players)")){
        qDebug() << q.lastError();
        qDebug() << "Error on MIN";
    }
    while (q.next()){
        return (q.value(0).toInt() - 1);
    }
    return -1;
}

void GameServer::ReportReady()
{
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
                    if (!qq.exec("UPDATE players SET ready = 1 WHERE UID=" + str.left(1))) {
                        qDebug() << qq.lastError();
                        qDebug() << "Error on UPDATE";

                     }
                }
                else if ((ind = str.indexOf(" NotReady")) != -1){
                    qDebug() << str.left(1);
                    QSqlQuery qq;
                    if (!qq.exec("UPDATE players SET ready = 0 WHERE UID=" + str.left(1))) {
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
void GameServer::UpdateReady() {

//    qDebug() << "Updating";
    QSqlQuery socks;
    if (!socks.exec("SELECT UID FROM players")) {
       qDebug() << socks.lastError();
       qDebug() << "Error on INSERT";

    }
//    qDebug() << "Updating";
    while(socks.next()){
        QTcpSocket* sock = playerSockets[socks.value(0).toInt()-1];
        qDebug() << sock;
        QByteArray block;
        QTextStream out(&block, QIODevice::WriteOnly);
        QSqlQuery info("SELECT UID, ready FROM players");
        while (info.next()){
            out << "Number: " << QString::number(info.value(0).toInt()) << endl
                << "Ready: " << QString::number(info.value(1).toInt()) << endl;
        }
//        sock = players[i]->socket;
        sock->write(block);
        qDebug() << block;


    }
}
