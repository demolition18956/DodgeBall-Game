#ifndef LOBBY_H
#define LOBBY_H

#include <QMainWindow>
#include <QHostAddress>
#include <QNetworkInterface>
#include <QtDebug>
#include <QUdpSocket>

namespace Ui {
class lobby;
}

class lobby : public QMainWindow
{
    Q_OBJECT

public:
    explicit lobby(QWidget *parent = nullptr);
    ~lobby();

private:
    Ui::lobby *ui;
    QUdpSocket* hostUDP;
    QString playerName;

private slots:
    void processMessage();
};

#endif // LOBBY_H
