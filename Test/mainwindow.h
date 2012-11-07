#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>
#include <QtNetwork\QTcpServer>
#include <QtNetwork\QTcpSocket>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(bool _isChild, QTcpServer *_TcpServer,QTcpSocket *_TcpSocket, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();

    void on_ButtonWrite_clicked();

    void on_ButtonRead_clicked();

    void acceptConnection();
    void ReadFromChild();
    void ReadFromParent();
    void displayError(QAbstractSocket::SocketError);


public:
    void writeToOtherSlow(const  char *data,int len);
    void writeToOtherSlow(const unsigned char *data,int len)
        {writeToOtherSlow((const char *)data,len);}

    void writeLog(QString what, QString message);

private:


    bool isChild;
    Ui::MainWindow *ui;

    QTcpServer *tcpServer;
    QTcpSocket *tcpClient;
    QTcpSocket *tcpServerConnection;




    void initBinProtocol();
    void receiveOneByte(char b);
};

#endif // MAINWINDOW_H
