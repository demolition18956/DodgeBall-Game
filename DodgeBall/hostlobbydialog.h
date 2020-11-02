#ifndef HOSTLOBBYDIALOG_H
#define HOSTLOBBYDIALOG_H

#include <QDialog>

namespace Ui {
class HostLobbyDialog;
}

class HostLobbyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit HostLobbyDialog(QWidget *parent = nullptr);
    ~HostLobbyDialog();

private:
    Ui::HostLobbyDialog *ui;
};

#endif // HOSTLOBBYDIALOG_H
