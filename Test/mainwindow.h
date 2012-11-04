#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(bool _isChild, QWidget *parent = 0);
    ~MainWindow();
    
private slots:
    void on_pushButton_clicked();

private:
    bool isChild;
    Ui::MainWindow *ui;
    void WriteLog(QString what, QString message);
};

#endif // MAINWINDOW_H
