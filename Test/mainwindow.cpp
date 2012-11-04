#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->tableWidget->setColumnCount(3);
    ui->tableWidget->setHorizontalHeaderItem(0,new QTableWidgetItem(tr("Время")));
    ui->tableWidget->setHorizontalHeaderItem(1,new QTableWidgetItem(tr("Событие")));
    ui->tableWidget->setHorizontalHeaderItem(2,new QTableWidgetItem(tr("Сообщение")));
    ui->tableWidget->horizontalHeader()->setResizeMode(2, QHeaderView::Stretch);
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
