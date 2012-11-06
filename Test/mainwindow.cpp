#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QtCore>
#include <iostream>
#include <QMessageBox>
#include <QScrollBar>
#include "../BinProtocol/BinProtocol.h"


MainWindow::MainWindow(bool _isChild, QTcpServer *_TcpServer, QTcpSocket *_TcpSocket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isChild = _isChild;
    tcpServer = _TcpServer;
    tcpClient = _TcpSocket;
    tcpServerConnection = NULL;
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Время")));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Событие")));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Сообщение")));
    ui->tableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    QRect r = frameGeometry();
    int sdvig = ((isChild?1:-1)*(10 + frameGeometry().width()/2 ) );
    QRect np(r.width()*3/2+sdvig,r.height()/2,r.width(),r.height());
    setGeometry(np);
    //move(pos().x + (isChild?1:0)*(10 + frameGeometry().width()), pos().y());


    if(tcpServer)connect(tcpServer, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
    if(tcpClient)connect(tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    if(tcpClient)connect(tcpClient, SIGNAL(readyRead()),this, SLOT(ReadFromParent()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_clicked()
{
    WriteLog("debug","Нажали");
}

void MainWindow::WriteLog(QString what, QString message)
{
    int r = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(r+1);
    ui->tableWidget->setItem(r,0,new QTableWidgetItem(QDateTime::currentDateTime().toString("hh:mm:ss.zzz")));
    ui->tableWidget->setItem(r,1,new QTableWidgetItem(what));
    ui->tableWidget->setItem(r,2,new QTableWidgetItem(message));
    ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->maximum());
}


void MainWindow::on_ButtonWrite_clicked()
{
    if(isChild){
        tcpClient->write("childW",6);
    }else{
        if(tcpServerConnection)tcpServerConnection->write("parrentW",8);
    }

}

void MainWindow::on_ButtonRead_clicked()
{
    QString t = "BugagaText";
    writeToOtherSlow(t.toAscii(),strlen(t.toAscii()));
}

void MainWindow::acceptConnection()
{
    tcpServerConnection = tcpServer->nextPendingConnection();
         connect(tcpServerConnection, SIGNAL(readyRead()),this, SLOT(ReadFromChild()));
         connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));

         WriteLog("tcp_server","Accepted connection");
         tcpServer->close();
}

void MainWindow::ReadFromChild()
{
    char data[200];
    memset(data,0,sizeof(data));
    int s;

    s = tcpServerConnection->read(data,sizeof(data)-1);

    //Передача полученных байт в бин-протокол
    for(int i=0;i<s;i++)receiveOneByte(data[i]);

    WriteLog("Read",QString("Read %1 bytes: %2").arg(s).arg(QString(data)));


}

void MainWindow::ReadFromParent()
{
    char data[200];
    memset(data,0,sizeof(data));
    int s;

    s = tcpClient->read(data,sizeof(data)-1);

    //Передача полученных байт в бин-протокол
    for(int i=0;i<s;i++)receiveOneByte(data[i]);

    WriteLog("Read",QString("Read %1 bytes: %2").arg(s).arg(QString(data)));

}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError) return;

    QMessageBox::information(this, tr("Network error"),
                             tr("The following error occurred: %1.")
                             .arg(tcpClient->errorString()));

    tcpClient->close();
    tcpServer->close();
}

void MainWindow::writeToOtherSlow(const char *data, int len)
{
    QTcpSocket *sock = isChild?tcpClient:tcpServerConnection;
    if(!sock)return;
    for(;len>0;len--){
        sock->write(data++,1);
        sock->flush();

        QEventLoop loop;
        QTimer::singleShot(10, &loop, SLOT(quit()));
        loop.exec();
    }
}


//Выделяемая для BinProtocol память.
static char receivedBuffer[1024];

//Грязный хак перехода от классовой модели к функциональной-контроллерной
//глобвальная переменная с указателем на класс, могущий переслать данные
extern MainWindow *LinkToSendClass;


void globalSendCharToExternal(unsigned char c)
{
    LinkToSendClass->writeToOtherSlow(&c,1);
}

void MainWindow::initBinProtocol()
{
    //Будут приниматься пакеты только с адресом 1 или 255.
    int serial = 1;

    //Необходимо вызвать в качестве инициализации буфера, серийника, отсылающей функции - возвращает true если все параметры в норме и false если нет
    //основная инициализирующая функция. На входе буфер для приема пакетов, серийник и отсылающая функция
    BP_Init_Protocol(receivedBuffer,sizeof(receivedBuffer),serial,globalSendCharToExternal);

}

void MainWindow::receiveOneByte(char b)
{
    BP_ReceiveChar(b);
}


