#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QTcpServer>
#include <player.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQueryModel>

class GameServer : public QTcpServer
{

public:
    explicit GameServer(QObject* parent = nullptr);
    int GetPlayerCount();
    bool isInLobby();
    ~GameServer();
    void playerJoined();

private:
    Player* players[6];   // array of 6 player objects
    int playerCount;
    bool inLobby;   // is the server currently in lobby or playing?
    bool addNewPlayer(Player &p);   // attempts to add a new player to the lobby
    QSqlDatabase db;
    void UpdateClients();
    QTcpSocket* playerSockets[6] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    int getMinSocket();
    void UpdateReady();

private slots:
    void StartGame();   // starts game when AllReady() is emitted
    void ProcessNewConnections();
    void ReportReady();


signals:
    void AllReady();   // emitted when all players are ready
};

#endif // GAMESERVER_H
