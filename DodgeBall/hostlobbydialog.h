#ifndef HOSTLOBBYDIALOG_H
#define HOSTLOBBYDIALOG_H

#include <QDialog>
#include <QUdpSocket>

namespace Ui {
class HostLobbyDialog;
}

class HostLobbyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HostLobbyDialog(QWidget *parent = nullptr);
    //set bool fuuncitons
    bool getBool();
    void setBool(bool value);
    ~HostLobbyDialog();

private:
    Ui::HostLobbyDialog *ui;
    bool back; //variable to show the main window again if hosting is cancelled
    QUdpSocket* hostUDP;


private slots:
    void cancel();
    void processMessage();

};

#endif // HOSTLOBBYDIALOG_H
