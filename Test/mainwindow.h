#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QtCore>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(bool _isChild, QProcess *_prgChild, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();

    void on_ButtonWrite_clicked();

    void on_ButtonRead_clicked();

private:
    bool isChild;
    QProcess *prgChild;
    Ui::MainWindow *ui;
    void WriteLog(QString what, QString message);
};

#endif // MAINWINDOW_H
