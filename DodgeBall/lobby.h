#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include "gameserver.h"
#include <QHostAddress>
#include <QNetworkInterface>
#include <QtDebug>
#include <QTcpSocket>
#include <QMessageBox>

namespace Ui {
class lobby;
}

class lobby : public QMainWindow
{
    Q_OBJECT

public:
    explicit lobby(QHostAddress ipAddress, int portNumber, bool host_,QWidget *parent = nullptr);
    ~lobby();
    bool isHost();
    void connection_error();
    bool getConnected();

private:
    Ui::lobby *ui;
    QString playerName;
    bool host;   // is the player the host?
    QTcpSocket socket;
    int playersJoined;
    QHostAddress address;
    int portNum;
    GameServer* server;
    int playeruid;
    bool connected;

signals:
    void ready();
    void showAgain();


public slots:
    void playerReady();
    void leave();

private slots:
    void processMessage();
    void initialConnect();
    void changeName();
};

#endif // LOBBY_H
