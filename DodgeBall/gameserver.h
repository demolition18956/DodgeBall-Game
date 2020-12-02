#ifndef GAMESERVER_H
#define GAMESERVER_H

#include <QTcpServer>
#include <player.h>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQueryModel>
#include <QTimer>

class GameServer : public QTcpServer
{

public:
    explicit GameServer(int portNum, QObject* parent = nullptr);
    int GetPlayerCount();
    bool isInLobby();
    ~GameServer();

private:
    int playerCount;
    bool inLobby;   // is the server currently in lobby or playing?
    bool addNewPlayer(Player &p);   // attempts to add a new player to the lobby
    QSqlDatabase db;
    void UpdateClients();
    QTcpSocket* playerSockets[6] = {nullptr,nullptr,nullptr,nullptr,nullptr,nullptr};
    int getMinSocket();
    void UpdateReady();
    QTimer* timer;

private slots:
    void StartGame();   // starts game when AllReady() is emitted
    void ProcessNewConnections();
    void ReportReady();
    void clientDisconnected();
    void sendAll(QString);
    void onTimeout();
    void ReportMovement();


signals:
    void AllReady();   // emitted when all players are ready
};

#endif // GAMESERVER_H
