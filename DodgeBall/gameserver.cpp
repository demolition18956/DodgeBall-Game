#include "gameserver.h"
#include <QTcpSocket>

GameServer::GameServer()
{
    setMaxPendingConnections(6);
    connect(this, SIGNAL(newConnection()),this, SLOT(ProcessNewConnections()));
    listen(QHostAddress::Any,5678);
}

void GameServer::ProcessNewConnections()
{
    while(hasPendingConnections())
    {
        if(playerCount >= 6)
        {
            // reject connection code here
            return;
        }
        for(int i=0;i<6;i++)
        {
            if(players[i])
            {
                continue;   // this player slot is already taken
            }
            else
            {
                players[i] = new Player();
            }
        }
    }
}
