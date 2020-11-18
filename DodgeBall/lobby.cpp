#include "lobby.h"
#include "ui_lobby.h"
#include <QRegularExpression>
#include <QRegularExpressionValidator>

lobby::lobby(QHostAddress ipAddress, int portNumber, bool host_,QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::lobby)
{
    ui->setupUi(this);
    connect(ui->readyButton, SIGNAL(pressed()), this, SLOT(playerReady()));
    const QHostAddress &localhost = QHostAddress(QHostAddress::LocalHost);
    qDebug() << QNetworkInterface::interfaceFromIndex(1).name();
    for (const QHostAddress &address: QNetworkInterface::allAddresses()) {
        if (address.protocol() == QAbstractSocket::IPv4Protocol && address != localhost)
             qDebug() << address.toString();
    }
    portNum = portNumber;
    address = QHostAddress(ipAddress);
    host = host_;
    connect(&socket, SIGNAL(connected()), this, SLOT(initialConnect()));
    if(host ==  true)   // if player is the host
    {
        server = new GameServer();
        socket.connectToHost(server->serverAddress(), portNum);   // open tcp socket on local machine (where server should be running)

    }
    else
    {
       socket.connectToHost(ipAddress,portNumber);   // non-host connection
       if(!socket.waitForConnected(5000))
       {
           int ret = QMessageBox::warning(this, tr("My Application"),
                                          tr("Couldn´t connect to server, try again."));

           qDebug() << "CLIENT: Couldn't Connect due to errors";
           connected = false;
           return;
       }
       else
       {
          this->show();
          connected = true;
       }
    }
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

void lobby::processMessage(){
    qDebug() << "Message Processing";
    QByteArray datagram;
    QString msg;
    QTextStream in(&socket);
    int ind, playNum;
    bool ok;
    while (in.readLineInto(&msg)){
        qDebug() << msg;
        int num = msg.toInt(&ok);
        if (ok){
            if (num != 0){
                qDebug() << "UID set";
                playeruid = num;
            }
            else {
                // Full Lobby Error
                break;
            }
        }
        qDebug() << msg;
        if ((ind = msg.indexOf("Number: ")) != -1){
            ind += 8;
            playNum = msg.right(msg.length() - ind).toInt(&ok);
            qDebug() << "Number " << msg.right(msg.length() - ind) << ok;
        }
        else if ((ind = msg.indexOf("Player Name: ")) != -1) {
            ind += 13;
            QString playName = msg.right(msg.length() - ind);
            qDebug() << "Name " << playName;
            switch(playNum){
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
                qDebug() << "That's illegal";
            }
        }
        else if ((ind = msg.indexOf("Ready: ")) != -1){
            ind += 7;
            int ready = msg.right(msg.length() - ind).toInt(&ok);
            qDebug() << "Ready " << msg.right(msg.length() - ind) << ok;
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
        else if ((ind = msg.indexOf("Joined: ")) != -1){
            ind += 8;
            int readySum = msg.right(msg.length() - ind).toInt(&ok);
            ui->playercountLabel->setNum(readySum);
        }
        else if ((ind = msg.indexOf("start")) != -1){
            qDebug() << "Starting Game!";
            map.setWindowState(Qt::WindowFullScreen);
            map.show();
        }

        // Either Player info or Ball info
        QTextStream message(&msg, QIODevice::ReadOnly);
        QString buffer;
        message >> buffer;
        qDebug() << "BUFFER: " << buffer;


        // Read Player Information (packet layout-->"PLAYER: uid team x y hasBall pixmap")
        if(buffer == "PLAYER:")
        {
            qDebug() << "WE are in";
            int uid;
            QString team;
            int x;
            int y;
            bool hasBall;
//            QByteArray pixmap;
            buffer.clear();
            message >> buffer;  // uid read
            uid = buffer.toInt();
            buffer.clear();
            message >> buffer;  // team read
            team = buffer;
            buffer.clear();
            message >> buffer;   // x pos read
            x = buffer.toInt();
            buffer.clear();
            message >> buffer;   // y pos read
            y = buffer.toInt();
            buffer.clear();
            message >> buffer;   // hasBall read
            hasBall = buffer.toInt();
            buffer.clear();
//            message >> buffer;   // pixmap read
//            qDebug() << buffer;
//            //QTextStream pix(&buffer, QIODevice::ReadOnly);  // text stream to readyfrom buffer(QString) to pixmap(ByteArray)
//            //pix >> pixmap;
//            pixmap = buffer.toUtf8();
//            buffer.clear();
            qDebug() << "Player Data Read: ";
            qDebug() << "UID: " << uid;
            qDebug() << "Team: " << team;
            qDebug() << "x: " << x;
            qDebug() << "y: " << y;
            qDebug() << "hasBall: " << hasBall;
//            qDebug() << "pixmap: " << pixmap;
        }
    }
    qDebug() << "CLIENT MESSAGE: " << msg;
}

bool lobby::isHost()
{
    return host;
}

void lobby::initialConnect()
{
    connect(&socket, SIGNAL(readyRead()),this, SLOT(processMessage()));
    if(socket.waitForReadyRead(5000))
    {
        QTextStream incoming(&socket);
        QString msg;
        incoming >> msg;
        int data = msg.toInt();
        if(data == 1)
        {
            qDebug() << "CLIENT: connection accepted!";
        }
//        else if(data == 0)
//        {
//            qDebug() << "CLIENT: connection refused!";
//            socket.abort();
//        }
        else
        {
            qDebug() << "CLIENT: something went wrong!";
        }
    }
    else
    {
        qDebug() << "CLIENT: Never got a message!";
    }

    connect(&socket, SIGNAL(readyRead()),this, SLOT(processMessage()));


}

void lobby::playerReady()
{
    static bool readyPrev = false;
    qDebug("Player ready");
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

void lobby::leave(){
    int button = QMessageBox::question(this, "Confirm Drop",
                                       "Are you sure you sure you want to leave?",
                                       QMessageBox::Yes, QMessageBox::No);


    if(button == QMessageBox::Yes){
        this->deleteLater();
        if(host == true) server->close();
        emit showAgain();
    }

}

void lobby::changeName(){
    QString msg = ui->nameEdit->text();
    if (msg.isEmpty())
        return;
    ui->nameEdit->clear();
    QByteArray block;
    QTextStream out(&block, QIODevice::WriteOnly);
    out << QString::number(playeruid) << " Name " << msg << endl;

    socket.write(block);
}

bool lobby::getConnected(){
    return connected;
}
