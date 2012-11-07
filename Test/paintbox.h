#ifndef PAINTBOX_H
#define PAINTBOX_H

#include <QWidget>
#include <QMouseEvent>
#include <QTimer>
#include "mainwindow.h"

class PaintBox : public QWidget
{
    Q_OBJECT
public:

    //�������� ������ � ������� ���������
    struct HoldObject{
        int x,y;
        int radius;
        QColor color;
    };
    typedef QList<HoldObject> ListHoldObjects;

    explicit PaintBox(QWidget *parent = 0);

    void ClearAll(bool isSendPack=true);
    
signals:
    
public slots:
    void timerWhenPressed();
    void timerCheckPacks();

protected:
    void paintEvent(QPaintEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent ( QMouseEvent * event );

    //������ ��� ������� � ��������� �������
    void drawObjects(QPainter *qp, const QRect &rect);
    //������ �������
    void drawPalitra(QPainter *qp);

    //��������� ����������� ������� �� Painter
    void drawOneObject(QPainter *qp,HoldObject &obj);

    //���� ���� ������� � ����������� �� ���������
    QColor getColorByXY(int x, int y);

    //�������� ������� ���������� ������� � ���������� ������� �������
    void updateNearObject(HoldObject &obj);

    //������ ���� ������������ ��������
    ListHoldObjects listHO;
    //������ ���������� �����
    QTimer *timerPress;

    //������� ������������� ������
    HoldObject currentObject;

    //����� ��� �� ������� �����
    void writeLog(QString what, QString message);
};

#endif // PAINTBOX_H
