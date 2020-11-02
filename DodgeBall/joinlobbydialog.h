#ifndef JOINLOBBYDIALOG_H
#define JOINLOBBYDIALOG_H

#include <QDialog>

namespace Ui {
class JoinLobbyDialog;
}

class JoinLobbyDialog : public QDialog
{
    Q_OBJECT

public:
    explicit JoinLobbyDialog(QWidget *parent = nullptr);
    ~JoinLobbyDialog();

private:
    Ui::JoinLobbyDialog *ui;

private slots:
    void cancel();
};

#endif // JOINLOBBYDIALOG_H
