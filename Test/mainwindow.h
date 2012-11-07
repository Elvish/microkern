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
    explicit MainWindow(bool _isChild, QTcpServer *_TcpServer,
                        QTcpSocket *_TcpClient, QWidget *parent = 0);
    ~MainWindow();
    
private slots:


    void acceptConnection();//��� ����������� �������
    void ReadFromChild();//��� ������ ��������� �������
    void ReadFromParent();//��� ������ �������� ��������
    void displayError(QAbstractSocket::SocketError);//����� ������ ��� ������ � �����


    void on_pushButtonClear_clicked();
    void on_ButtonSendRubbish_clicked();

public:
    //������ � ����� ������ ������
    void writeToOtherSlow(const  char *data,int len);
    //�� ��, ����� ��� ��������� � ����������� �����
    void writeToOtherSlow(const unsigned char *data,int len);

    //����� ��������� �� ����� ��� �������
    void writeLog(QString what, QString message);

    //���������� ������� ������ �� ������.
    //��� ���������� �������� � receiveOneByte
    void ReadFromSocket(QTcpSocket *sock);

private:


    bool isChild;
    Ui::MainWindow *ui;

    QTcpServer *tcpServer;
    QTcpSocket *tcpClient;
    QTcpSocket *tcpServerConnection;

    //������������� BinProtocol
    void initBinProtocol();
    //�������� ����� ������ � BinProtocol
    void receiveOneByte(char b);
};

#endif // MAINWINDOW_H
