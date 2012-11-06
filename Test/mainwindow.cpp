#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QtCore>
#include <iostream>
#include <QMessageBox>


MainWindow::MainWindow(bool _isChild, QProcess *_prgChild, QTcpServer *_TcpServer, QTcpSocket *_TcpSocket, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isChild = _isChild;
    prgChild = _prgChild;
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

    //bytesReceived += (int)tcpServerConnection->bytesAvailable();
    s = tcpServerConnection->read(data,sizeof(data)-1);

    WriteLog("Read",QString("Read %1 bytes: %2").arg(s).arg(QString(data)));


}

void MainWindow::ReadFromParent()
{
    char data[200];
    memset(data,0,sizeof(data));
    int s;

    s = tcpClient->read(data,sizeof(data)-1);

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


