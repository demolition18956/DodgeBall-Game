#include "gameserver.h"
#include <QTcpSocket>
#include <QByteArray>

GameServer::GameServer()
{
    setMaxPendingConnections(6);
    connect(this, &QTcpServer::newConnection, this, &GameServer::ProcessNewConnections);
    listen(QHostAddress::Any,5678);
}

void GameServer::ProcessNewConnections()
{
    while(hasPendingConnections())
    {
        if(playerCount >= 6)
        {
            QTcpSocket* sock = nextPendingConnection();
            QByteArray ba;
            ba.setNum(0);   // 0 for Rejected
            sock->write(ba);
            sock->abort();
            delete sock;
            return;
        }
        for(int i=0;i<6;i++)
        {
            if(players[i]->getSocket())
            {
                continue;   // this player slot is already taken
            }
            else
            {
                QTcpSocket* sock = nextPendingConnection();
                players[i] = new Player();
                players[i]->setSocket(sock);
                playerCount++;
                QByteArray ba;
                ba.setNum(1);   // 1 for Accepted
                players[i]->getSocket()->write(ba);
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
