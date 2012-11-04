#include <QtGui/QApplication>
#include <QtCore>
#include "mainwindow.h"

int main(int argc, char *argv[])
{
    QTextCodec::setCodecForLocale( QTextCodec::codecForName("ANSI") );
    QTextCodec::setCodecForCStrings( QTextCodec::codecForLocale() );
    QTextCodec::setCodecForTr( QTextCodec::codecForLocale() );

    QApplication a(argc, argv);
    bool isChild = false;

    QProcess me;
    if(!a.arguments().contains("-child")){
        QStringList argums;
        argums << "-child";
        qDebug()<<"We are parent...";
        me.start(a.applicationFilePath(),argums);
        me.waitForStarted(1000);
        //спим 100 мс, чтобы наше окно было созданно позже
        QEventLoop loop;
        QTimer::singleShot(100, &loop, SLOT(quit()));
        loop.exec();

        me.open(QIODevice::ReadWrite);
    }else{
        qDebug()<<"We are child...";
        isChild = true;




    }

    MainWindow w(isChild,&me);
    w.show();



    int ret = a.exec();



    return ret;
}
