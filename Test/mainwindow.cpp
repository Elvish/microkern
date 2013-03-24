#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QtCore>
#include <iostream>
#include <QMessageBox>
#include <QScrollBar>
#include "../BinProtocol/BinProtocol.h"


MainWindow::MainWindow(bool _isChild, QTcpServer *_TcpServer, QTcpSocket *_TcpClient, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isChild = _isChild;
    tcpServer = _TcpServer;
    tcpClient = _TcpClient;
    tcpServerConnection = NULL;
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Время")));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Что")));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Сообщение")));
    ui->tableWidget->setColumnWidth(0, 60);
    ui->tableWidget->setColumnWidth(1, 40);
    ui->tableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
    QRect r = frameGeometry();
    int sdvig = ((isChild?1:0)*(30 + frameGeometry().width() ) );
    QRect np(50+sdvig,r.height()/2,r.width(),r.height());
    setGeometry(np);
    //move(pos().x + (isChild?1:0)*(10 + frameGeometry().width()), pos().y());

    //Инициализация BinProtocol
    initBinProtocol();

    //Настраиваем сокеты
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
    //вот такой вот нынче мусор
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
    char data[2048];
    int s = sock->read(data,sizeof(data)-1);

    //Передача полученных байт в бин-протокол через посредника receiveOneByte
    for(int i=0;i<s;i++){
        if(ui->checkBoxShowBytes->checkState() == Qt::Checked)writeLog("Read",QString("< 0x%1").arg((int)(unsigned char)data[i],2,16,QLatin1Char( '0' )));
        receiveOneByte(data[i]);
    }
}

void MainWindow::displayError(QAbstractSocket::SocketError socketError)
{
    if (socketError == QTcpSocket::RemoteHostClosedError) return;

    QMessageBox::information(this, tr("Сетевая ошибка"),
                             tr("Непонятки с сокетами: %1.")
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
        //Отправляем по одному символу сразу, для "усложнения" разгребания пакетов
        sock->flush();

        //При желании можно сделать передачу медленной...
        //QEventLoop loop;
        //QTimer::singleShot(10, &loop, SLOT(quit()));
        //loop.exec();
    }
}

void MainWindow::writeToOtherSlow(const unsigned char *data, int len)
{
    writeToOtherSlow((const char *)data,len);
}


void MainWindow::on_pushButtonClear_clicked()
{
    ui->paintBox->ClearAll();
}


//--------------------------------------------------------------------
// Собственно нижний уровень работы с BinProtocol
//--------------------------------------------------------------------

//Выделяемая для BinProtocol память.
static char receivedBuffer[1024];

//Грязный хак перехода от классовой модели к функциональной-контроллерной
//глобвальная переменная с указателем на класс, могущий переслать данные
extern MainWindow *LinkToSendClass;


extern "C" void globalSendCharToExternal(unsigned char c)
{
    LinkToSendClass->writeToOtherSlow(&c,1);
}

void MainWindow::initBinProtocol()
{
    //Будут приниматься пакеты только с адресом 1 или 255.
    int serial = 1;

    //Необходимо вызвать в качестве инициализации буфера, серийника, отсылающей функции - возвращает true если все параметры в норме и false если нет
    //основная инициализирующая функция. На входе буфер для приема пакетов, серийник и отсылающая функция
    bool res = BP_Init_Protocol(receivedBuffer,sizeof(receivedBuffer),serial,globalSendCharToExternal);
    writeLog("INIT",res?"Ok":"Fail");

}

//Передача потока байт в BinProtocol
void MainWindow::receiveOneByte(char b)
{
    BP_ReceiveChar(b);
}


