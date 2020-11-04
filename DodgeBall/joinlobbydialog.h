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
    //set bool fuuncitons
    bool getBool();
    void setBool(bool value);
    ~JoinLobbyDialog();

private:
    Ui::JoinLobbyDialog *ui;
    bool back;
private slots:
    void cancel();
};

#endif // JOINLOBBYDIALOG_H
