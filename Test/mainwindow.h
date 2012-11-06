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
    explicit MainWindow(bool _isChild, QProcess *_prgChild, QTcpServer *_TcpServer,QTcpSocket *_TcpSocket, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();

    void on_ButtonWrite_clicked();

    void on_ButtonRead_clicked();

    void acceptConnection();
    void ReadFromChild();
    void ReadFromParent();
    void displayError(QAbstractSocket::SocketError);

private:
    bool isChild;
    QProcess *prgChild;
    Ui::MainWindow *ui;
    void WriteLog(QString what, QString message);

    QTcpServer *tcpServer;
    QTcpSocket *tcpClient;
    QTcpSocket *tcpServerConnection;

};

#endif // MAINWINDOW_H
