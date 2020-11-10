#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QTcpServer>
#include <player.h>

class GameServer : public QTcpServer
{

private:
    Player* players[6];   // array of 6 player objects
    int playerCount;
    bool inLobby;   // is the server currently in lobby or playing?
    bool addNewPlayer(Player &p);   // attempts to add a new player to the lobby

private slots:
    void StartGame();   // starts game when AllReady() is emitted
    void ProcessNewConnections();

public:
    GameServer();
    int GetPlayerCount();
    bool isInLobby();
    ~GameServer();
    void playerJoined(int n);

signals:
    void AllReady();   // emitted when all players are ready
};

#endif // GAMESERVER_H
