#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include <quitdialog.h>
#include <hostlobbydialog.h>
#include <joinlobbydialog.h>
#include <lobby.h>
#include <gameserver.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
    void hostNew();
    void hostFunc();
    void joinNew(QString ipAddress, int portNumber);
    void exitFunc();
    void joinFunc();
    void showBack();

private slots:
    void exit();

private:
    Ui::MainWindow *ui;
    QuitDialog *quitDialog;
    HostLobbyDialog *hostDialog;
    JoinLobbyDialog *joinDialog;
    GameServer* server;
    lobby* gameLobby;

};

#endif // MAINWINDOW_H
