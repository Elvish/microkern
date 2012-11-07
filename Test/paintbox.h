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

    struct HoldObject{
        int x,y;
        int radius;
        QColor color;
    };
    typedef QList<HoldObject> ListHoldObjects;

    explicit PaintBox(QWidget *parent = 0);

    void ClearAll();
    
signals:
    
public slots:
    void timerWhenPressed();
    void timerCheckPacks();

protected:
    void paintEvent(QPaintEvent *event);
    void drawObjects(QPainter *qp, const QRect &rect);
    void drawPalitra(QPainter *qp);

    void drawOneObject(QPainter *qp,HoldObject &obj);

    QColor getColorByXY(int x, int y);


    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent ( QMouseEvent * event );

    void updateNearObject(HoldObject &obj);


    ListHoldObjects listHO;
    QTimer *timerPress;
    QColor currentColor;

    HoldObject currentObject;


    void writeLog(QString what, QString message);
};

#endif // PAINTBOX_H
