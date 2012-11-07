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
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("�����")));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("���")));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("���������")));
    ui->tableWidget->setColumnWidth(0, 60);
    ui->tableWidget->setColumnWidth(1, 40);
    ui->tableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    QRect r = frameGeometry();
    int sdvig = ((isChild?1:0)*(30 + frameGeometry().width() ) );
    QRect np(50+sdvig,r.height()/2,r.width(),r.height());
    setGeometry(np);
    //move(pos().x + (isChild?1:0)*(10 + frameGeometry().width()), pos().y());

    initBinProtocol();

    if(tcpServer)connect(tcpServer, SIGNAL(newConnection()),this, SLOT(acceptConnection()));
    if(tcpClient)connect(tcpClient, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));
    if(tcpClient)connect(tcpClient, SIGNAL(readyRead()),this, SLOT(ReadFromParent()));



}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::writeLog(QString what, QString message)
{
    int r = ui->tableWidget->rowCount();
    ui->tableWidget->setRowCount(r+1);
    ui->tableWidget->setItem(r,0,new QTableWidgetItem(QDateTime::currentDateTime().toString("mm:ss.zzz")));
    ui->tableWidget->setItem(r,1,new QTableWidgetItem(what));
    ui->tableWidget->setItem(r,2,new QTableWidgetItem(message));
    ui->tableWidget->verticalScrollBar()->setValue(ui->tableWidget->verticalScrollBar()->maximum());
    ui->tableWidget->update();
}


void MainWindow::on_ButtonSendRubbish_clicked()
{
    QString t = "BugagaText";
    writeToOtherSlow(t.toAscii(),strlen(t.toAscii()));
}

void MainWindow::acceptConnection()
{
    tcpServerConnection = tcpServer->nextPendingConnection();
         connect(tcpServerConnection, SIGNAL(readyRead()),this, SLOT(ReadFromChild()));
         connect(tcpServerConnection, SIGNAL(error(QAbstractSocket::SocketError)),this, SLOT(displayError(QAbstractSocket::SocketError)));

         writeLog("tcp_server","Accepted connection");
         tcpServer->close();
}

void MainWindow::ReadFromChild()
{
    ReadFromSocket(tcpServerConnection);
}

void MainWindow::ReadFromParent()
{
    ReadFromSocket(tcpClient);
}

void MainWindow::ReadFromSocket(QTcpSocket *sock)
{
    char data[200];
    memset(data,0,sizeof(data));
    int s;

    s = sock->read(data,sizeof(data)-1);

    //�������� ���������� ���� � ���-��������
    for(int i=0;i<s;i++){
        if(ui->checkBoxShowBytes->checkState() == Qt::Checked)writeLog("Read",QString("< 0x%1").arg((int)(unsigned char)data[i],2,16,QLatin1Char( '0' )));
        receiveOneByte(data[i]);
    }
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
        if(ui->checkBoxSendPrint->checkState() == Qt::Checked){
            writeLog("Send",QString("> 0x%2 (%1)").arg(len).arg((int)(unsigned char)*data,2,16,QLatin1Char( '0' )));
        }
        sock->write(data++,1);
        sock->flush();

        //QEventLoop loop;
        //QTimer::singleShot(10, &loop, SLOT(quit()));
        //loop.exec();
    }
}


//���������� ��� BinProtocol ������.
static char receivedBuffer[1024];

//������� ��� �������� �� ��������� ������ � ��������������-�������������
//����������� ���������� � ���������� �� �����, ������� ��������� ������
extern MainWindow *LinkToSendClass;


void globalSendCharToExternal(unsigned char c)
{
    LinkToSendClass->writeToOtherSlow(&c,1);
}

void MainWindow::initBinProtocol()
{
    //����� ����������� ������ ������ � ������� 1 ��� 255.
    int serial = 255;

    //���������� ������� � �������� ������������� ������, ���������, ���������� ������� - ���������� true ���� ��� ��������� � ����� � false ���� ���
    //�������� ���������������� �������. �� ����� ����� ��� ������ �������, �������� � ���������� �������
    bool res = BP_Init_Protocol(receivedBuffer,sizeof(receivedBuffer),serial,globalSendCharToExternal);
    writeLog("INIT",res?"Ok":"Fail");

}

void MainWindow::receiveOneByte(char b)
{
    BP_ReceiveChar(b);
}



void MainWindow::on_pushButtonClear_clicked()
{
    ui->paintBox->ClearAll();
}

