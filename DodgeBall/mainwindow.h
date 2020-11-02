#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPushButton>
#include <quitdialog.h>
#include <hostlobbydialog.h>
#include <joinlobbydialog.h>

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
    void hostFunc();
    void joinFunc();
    void exitFunc();

private slots:
    void exit();

private:
    Ui::MainWindow *ui;
    QuitDialog *quitDialog;
    HostLobbyDialog *hostDialog;
    JoinLobbyDialog *joinDialog;
};

#endif // MAINWINDOW_H
