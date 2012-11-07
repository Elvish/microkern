#ifndef PAINTBOX_H
#define PAINTBOX_H

#include <QWidget>
#include <QMouseEvent>
#include <QTimer>

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
    
signals:
    
public slots:
    void timerWhenPressed();

protected:
    void paintEvent(QPaintEvent *event);
    void drawLines(QPainter *qp);
    void drawPalitra(QPainter *qp);
    QColor getColorByXY(int x, int y);


    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent ( QMouseEvent * event );


    ListHoldObjects listHO;
    QTimer *timerPress;
    QColor currentColor;

};

#endif // PAINTBOX_H
