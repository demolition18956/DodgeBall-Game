#include "gameserver.h"
#include <QTcpSocket>
#include <QByteArray>

GameServer::GameServer()
{
    setMaxPendingConnections(6);
    connect(this, &QTcpServer::newConnection, this, &GameServer::ProcessNewConnections);
    if(!this->listen(QHostAddress::Any,5678)){
        qDebug() << "Could not start server";
    }
    else{
        qDebug() << "Listening";
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
        qDebug() << "processing incoming connection";
        if(playerCount >= 6)
        {
            QTcpSocket* sock = nextPendingConnection();
            QByteArray ba;
            ba.setNum(0);   // 0 for Rejected
            sock->write(ba);
            sock->abort();
            delete sock;
            qDebug() << "player rejected!";
            return;
        }
        for(int i=0;i<6;i++)
        {
            qDebug() << "getting socket";
            if(players[i]->getSocket())
            {
                continue;   // this player slot is already taken
            }
            else
            {
                QTcpSocket* sock = nextPendingConnection();
                players[i]->setSocket(sock);
                players[i]->getSocket()->write("1");
                playerCount++;
                

                qDebug() << "player accepted!";
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
