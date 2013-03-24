#include <QtGui/QApplication>
#include <QtCore>
//#include <QTcpServer>
#include <QtNetwork\QTcpServer>
#include <QtNetwork\QTcpSocket>
#include "mainwindow.h"
#include <QMessageBox>

//Чтобы без заморочек ссылаться на наше окошко
MainWindow *LinkToSendClass=NULL;

int main(int argc, char *argv[])
{
    //Подрубаем кириллицу
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("UTF8") );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
    QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );

    QApplication a(argc, argv);
    bool isChild = false;

    QTcpServer tcpServer;
    QHostAddress localHost("127.0.0.1");
    QTcpSocket tcpClient;


    QStringList myarg = a.arguments();

    QHostAddress host = QHostAddress::Any;//localHost;
    quint16 port=0;
    QProcess me;//когда кончится область видимости - тогда и прога канет всуе


    if(!myarg.contains("-child")){
        //Мы родитель или сервер
        //Открывает сокет, запускаем ребенка
        QStringList argums;
        argums << "-child";
        qDebug()<<"We are parent...";

        tcpServer.listen();
        argums << "-A:" + localHost.toString();
        argums << QString("-P:%1").arg(tcpServer.serverPort());

        qDebug()<<argums;

        //С этим ключем мы не запускаем ребенка
        if(!myarg.contains("-didicated"))me.start(a.applicationFilePath(),argums);
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

    MainWindow w(isChild,&tcpServer,&tcpClient);
    LinkToSendClass = &w;
    w.show();

    //пишем порт, который слушаем на форму, чтобы знать как подключаться
    w.writeLog("INIT",QString("Port:%1").arg(tcpServer.serverPort()));

    //воссоздаем семью из ребенка безпризорника и родителя партеногенезника
    if(isChild)tcpClient.connectToHost(host, port);


    int ret = a.exec();



    return ret;
}
