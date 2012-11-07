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


    void acceptConnection();//Для подключения ребенка
    void ReadFromChild();//Для чтения родителем ребенка
    void ReadFromParent();//для чтения ребенком родителя
    void displayError(QAbstractSocket::SocketError);//пишем ошибки при работе с сетью


    void on_pushButtonClear_clicked();
    void on_ButtonSendRubbish_clicked();

public:
    //Запись в сокет потока данных
    void writeToOtherSlow(const  char *data,int len);
    //то же, чтобы без заморочек с приведением типов
    void writeToOtherSlow(const unsigned char *data,int len);

    //Пишем сообщение на форму для отладки
    void writeLog(QString what, QString message);

    //Обобщенная функция чтения из сокета.
    //Все полученное передает в receiveOneByte
    void ReadFromSocket(QTcpSocket *sock);

private:


    bool isChild;
    Ui::MainWindow *ui;

    QTcpServer *tcpServer;
    QTcpSocket *tcpClient;
    QTcpSocket *tcpServerConnection;

    //Инициализация BinProtocol
    void initBinProtocol();
    //Передает новый символ в BinProtocol
    void receiveOneByte(char b);
};

#endif // MAINWINDOW_H
