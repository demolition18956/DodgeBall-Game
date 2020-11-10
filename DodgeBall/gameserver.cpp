#include "gameserver.h"
#include <QTcpSocket>
#include <QByteArray>

GameServer::GameServer()
{
    setMaxPendingConnections(6);
    connect(this, &QTcpServer::newConnection, this, &GameServer::ProcessNewConnections);
    if(!this->listen(QHostAddress::Any,5678)){
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
}

void GameServer::ProcessNewConnections()
{
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
        for(int i=0;i<6;i++)
        {
            qDebug() << "SERVER: getting socket";
            if(players[i]->socket->state() == QAbstractSocket::ConnectedState)
            {
                continue;   // this player slot is already taken
            }
            else
            {
                QTcpSocket* sock = this->nextPendingConnection();
                delete players[i]->socket;
                players[i]->socket = sock;
                players[i]->socket->write("1");   // 1 for Accepted
                players[i]->socket->flush();
                playerCount++;



                qDebug() << "SERVER: player accepted!";
                break;
            }
        }
    }
}

GameServer::~GameServer()
{
    for(int i=0;i<6;i++)
    {
        delete players[i];
    }
}

void GameServer::playerJoined(int playersCount)
{
    for(int i = 0; i < 6; i++)
    {
        if(players[i]->socket->state() == QAbstractSocket::ConnectedState)
        {
            players[i]->socket->write("2");
        }
    }

}
