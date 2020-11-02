#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDebug>
#include <QPushButton>

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

private:
    Ui::MainWindow *ui;
    QPushButton *mainhostbutton;
    QPushButton *mainjoinbutton;
    QPushButton *mainexitbutton;
};

#endif // MAINWINDOW_H
