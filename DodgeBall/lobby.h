#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QtDebug>
#include <QTcpSocket>

namespace Ui {
class lobby;
}

class lobby : public QMainWindow
{
    Q_OBJECT

public:
    explicit lobby(bool host_,QWidget *parent = nullptr);
    ~lobby();
    bool isHost();

private:
    Ui::lobby *ui;
    QString playerName;
    bool host;   // is the player the host?
    QTcpSocket socket;

private slots:
    void processMessage();
    void initialConnect();
};

#endif // LOBBY_H
