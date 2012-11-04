#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QtCore>

MainWindow::MainWindow(bool _isChild, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    isChild = _isChild;
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
