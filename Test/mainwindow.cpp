#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QtCore>
#include <iostream>


MainWindow::MainWindow(bool _isChild, QProcess *_prgChild, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isChild = _isChild;
    prgChild = _prgChild;
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
        //fprintf(stdout,"fprintf out ");
        std::cout << " cout ";
        std::cout.flush();
    }else{
        prgChild->write("Test1",5);
        prgChild->waitForBytesWritten(10);
    }

}

void MainWindow::on_ButtonRead_clicked()
{
    char data[200];
    memset(data,0,sizeof(data));
    int s;
    if(isChild){

        QFile stdIn;
        stdIn.open(stdin, QIODevice::ReadOnly);
        s =stdIn.read(data,sizeof(data)-1);

        /*

        QTextStream tin(stdin);
        //QDataStream in(tin.device());
        //s = in.readRawData(data,sizeof(data)-1);

        if(tin.atEnd()){
            WriteLog("Read","END");
            return;
        }

        std::cout << " before ";
        std::cout.flush();

        QString str = tin.read(50);

        std::cout << " after ";
        std::cout.flush();

        s = strlen(str.toAscii());
        memcpy(data,str.toAscii(),s);*/
    }else{
        s = prgChild->read(data,sizeof(data)-1);
    }
    WriteLog("Read",QString("Read %1 bytes: %2").arg(s).arg(QString(data)));
}



