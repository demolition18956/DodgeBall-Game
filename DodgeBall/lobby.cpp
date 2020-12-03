#include "lobby.h"
#include "ui_lobby.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>
#include <QNetworkProxy>

//*************************************************************************************************//
//                                      Constructor                                                //
//*************************************************************************************************//

lobby::lobby(QHostAddress ipAddress, int portNumber, bool host_,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::lobby)
{
    ui->setupUi(this);
    connect(ui->readyButton, SIGNAL(pressed()), this, SLOT(playerReady()));

    // Set Up Client-to-Server Socket Connection
    portNum = portNumber;
    address = QHostAddress(ipAddress);
    host = host_;
    socket.setProxy(QNetworkProxy::NoProxy);
    if(host ==  true)   // if player is the host
    {
            server = new GameServer(portNumber, this);
            socket.connectToHost(server->serverAddress(), portNum);   // open tcp socket on local machine (where server should be running)

            if(!socket.waitForConnected(5000))
            {
                int ret = QMessageBox::warning(this, tr("My Application"),
                                               tr("Couldn´t create server, try again."));

                //qDebug() << "CLIENT: Couldn't Connect due to errors";
                connected = false;
                return;
            }

            else
            {
               this->show();
               connected = true;
            }
    }
    else
    {
       socket.connectToHost(ipAddress,portNumber);   // non-host connection
       if(!socket.waitForConnected(5000))
       {
           int ret = QMessageBox::warning(this, tr("My Application"),
                                          tr("Couldn´t connect to server, try again."));

           //qDebug() << "CLIENT: Couldn't Connect due to errors";
           connected = false;
           return;
       }
       else
       {
          this->show();
          connected = true;
       }
    }

    // Outputs IP Address(es) in Lobby UI
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    QString addy = "IP Address: ";
    if (ipAddress.toString() == localhost.toString()){
        bool more = false;
        for (const QHostAddress &address: QNetworkInterface::allAddresses())
        {
            if (address.isGlobal() && (address.protocol() == QAbstractSocket::IPv4Protocol))
            {
                 if (more)
                    addy.append(", ");
                 addy.append(address.toString());
                 more = true;
            }
        }
    }
    else {
        addy.append(ipAddress.toString());
    }

    ui->ipLabel->setText(addy);



    connect(ui->leaveButton, SIGNAL(clicked()), this, SLOT(leave()));
    connect(&socket, SIGNAL(readyRead()),this, SLOT(processMessage()));
    connect(ui->nameButton, SIGNAL(clicked(bool)), this, SLOT(changeName()));
    QRegularExpression nameInput ("[A-z]{1,10}");
    QRegularExpressionValidator* ipValidator = new QRegularExpressionValidator(nameInput, ui->nameEdit);
    ui->nameEdit->setValidator(ipValidator);
}


lobby::~lobby()
{

    delete ui;
    delete server;
}

//*************************************************************************************************//
//                                      Client Handling                                            //
//*************************************************************************************************//

void lobby::processMessage(){

    //qDebug() << "Message Processing";
    QByteArray datagram;
    QString msg;
    QTextStream in(&socket);
    int ind, playNum;
    bool ok;
    while (in.readLineInto(&msg)){

        // Just reading singular int sets PlayerUID for Client
        int num = msg.toInt(&ok);
        if (ok)
        {
            if (num != 0){
                playeruid = num;
                //qDebug() << "Player UID: " << playeruid;
            }
            else {
                // Full Lobby Error
                break;
            }
        }
        //qDebug() << msg;

        // Sets Number of player info (playNum) to be edited
        if ((ind = msg.indexOf("Number: ")) != -1)
        {
            ind += 8;

            playNum = msg.right(msg.length() - ind).toInt(&ok);
            //qDebug() << "Number " << msg.right(msg.length() - ind) << ok;
        }

        // Set Player Name of playNum
        else if ((ind = msg.indexOf("Player Name: ")) != -1)
        {
            ind += 13;

            QString playName = msg.right(msg.length() - ind);
            //qDebug() << "Name " << playName;

            switch(playNum)
            {
            case 1:
                ui->player1NameLabel->setText(playName);
                break;
            case 2:
                ui->player2NameLabel->setText(playName);
                break;
            case 3:
                ui->player3NameLabel->setText(playName);
                break;
            case 4:
                ui->player4NameLabel->setText(playName);
                break;
            case 5:
                ui->player5NameLabel->setText(playName);
                break;
            case 6:
                ui->player6NameLabel->setText(playName);
                break;
            default:
                break;//qDebug() << "That's illegal";
            }
        }

        // Set Ready Status of playNum
        else if ((ind = msg.indexOf("Ready: ")) != -1)
        {
            ind += 7;

            int ready = msg.right(msg.length() - ind).toInt(&ok);
            QString readyStr = (ready == 0) ? "Not Ready" : "Ready";

            switch(playNum){
            case 1:
                ui->player1ReadyLabel->setText(readyStr);
                break;
            case 2:
                ui->player2ReadyLabel->setText(readyStr);
                break;
            case 3:
                ui->player3ReadyLabel->setText(readyStr);
                break;
            case 4:
                ui->player4ReadyLabel->setText(readyStr);
                break;
            case 5:
                ui->player5ReadyLabel->setText(readyStr);
                break;
            case 6:
                ui->player6ReadyLabel->setText(readyStr);
                break;
            default:
                qDebug() << "That's illegal";
            }
        }

        // Set Total Number of Players in Lobby
        else if ((ind = msg.indexOf("Joined: ")) != -1)
        {
            ind += 8;
            int readySum = msg.right(msg.length() - ind).toInt(&ok);
            ui->playercountLabel->setNum(readySum);
        }

        // Start Game
        else if ((ind = msg.indexOf("start")) != -1)
        {
            //qDebug() << "Starting Game!";

            playerReady();
            map = new mapDialog(playeruid,this);
            connect(map, SIGNAL(finishGame(QString)), this, SLOT(GameFinish(QString)));
            map->SetSocket(&socket);
            map->setWindowState(Qt::WindowFullScreen);

            disconnect(&socket, SIGNAL(readyRead()),this, SLOT(processMessage()));
            map->show();
            return;
        }

    }
    //qDebug() << "CLIENT MESSAGE: " << msg;
}

bool lobby::isHost()
{
    return host;
}

//*************************************************************************************************//
//                                      Connections                                                //
//*************************************************************************************************//

// Player triggers Ready Status and Reports to Server
void lobby::playerReady()
{
    static bool readyPrev = false;
    //qDebug("Player ready");
    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    if (!readyPrev){
        out << QString::number(playeruid) << " Ready" << endl;
        ui->readyButton->setText("UnReady");
    }
    else {
        out << QString::number(playeruid) << " NotReady" << endl;
        ui->readyButton->setText("Ready");
    }

    readyPrev ^= true;
    socket.write(block);
}

// User Tries to Leave Lobby
void lobby::leave()
{
    int button = QMessageBox::question(this, "Confirm Drop",
        "Are you sure you sure you want to leave?",
            QMessageBox::Yes, QMessageBox::No);

    if(button == QMessageBox::Yes)
    {
        this->deleteLater();
        if(host == true) server->close();
        emit showAgain();
    }

}

// User Changes Lobby Name, and Reports New Name to Server
void lobby::changeName()
{
    QString msg = ui->nameEdit->text();
    if (msg.isEmpty())
        return;
    ui->nameEdit->clear();
    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << QString::number(playeruid) << " Name " << msg << endl;

    socket.write(block);
}

// Returns connected Boolean
bool lobby::getConnected()
{
    return connected;
}

// Triggered When Game Finishes and Users Returns To Lobby
void lobby::GameFinish(QString team)
{
    qDebug() << "CLIENT: " << team << " WINS!!!";
    connect(&socket, SIGNAL(readyRead()),this, SLOT(processMessage()));
    QString label = "Winner: ";
    label.append(team.toUpper());
    ui->winnerLabel->setText(label);
    map->close();
    delete map;
    this->show();
}
