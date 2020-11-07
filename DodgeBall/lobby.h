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
    QTcpSocket socket;
    bool isHost();

private:
    Ui::lobby *ui;
    QString playerName;
    bool host;

private slots:
    void processMessage();
};

#endif // LOBBY_H
