#include <QtGui/QApplication>
#include <QtCore>
//#include <QTcpServer>
#include <QtNetwork\QTcpServer>
#include <QtNetwork\QTcpSocket>
#include "mainwindow.h"
#include <QMessageBox>


int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("ANSI") );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
    QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );

    QApplication a(argc, argv);
    bool isChild = false;

    QTcpServer tcpServer;
    QHostAddress localHost("127.0.0.1");
    QTcpSocket tcpClient;


    QProcess me;
    QStringList myarg = a.arguments();

    QHostAddress host=localHost;
    quint16 port=0;


    if(!myarg.contains("-child")){
        QStringList argums;
        argums << "-child";
        qDebug()<<"We are parent...";

        tcpServer.listen(localHost);
        argums << "-A:" + tcpServer.serverAddress().toString();
        argums << QString("-P:%1").arg(tcpServer.serverPort());

        qDebug()<<argums;

        me.start(a.applicationFilePath(),argums);
        me.waitForStarted(1000);
        //спим 100 мс, чтобы наше окно было созданно позже
        //QEventLoop loop;
        //QTimer::singleShot(100, &loop, SLOT(quit()));
        //loop.exec();

        me.open(QIODevice::ReadWrite);
    }else{
        qDebug()<<"We are child...";
        isChild = true;

        QString h;
        foreach(QString str, myarg){
            if(str.startsWith("-A:")){
                host = h = str.mid(3);
            }
            if(str.startsWith("-P:")){
                port = str.mid(3).toInt();
            }
        }

        //спим 100 мс, чтобы наше окно было созданно позже
        QEventLoop loop;
        QTimer::singleShot(100, &loop, SLOT(quit()));
        loop.exec();


    }

    MainWindow w(isChild,&me,&tcpServer,&tcpClient);
    w.show();


    if(isChild)tcpClient.connectToHost(host, port);


    int ret = a.exec();



    return ret;
}
