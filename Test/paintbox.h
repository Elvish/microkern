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

    //Основной объект с которым оперируем
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

    //Рисуем все объекты в указанной области
    void drawObjects(QPainter *qp, const QRect &rect);
    //Рисуем палитру
    void drawPalitra(QPainter *qp);

    //Отрисовка конкретного объекта на Painter
    void drawOneObject(QPainter *qp,HoldObject &obj);

    //Дает цвет палитры в зависимости от координат
    QColor getColorByXY(int x, int y);

    //Вызывает событие обновления графики в окресности данного объекта
    void updateNearObject(HoldObject &obj);

    //Список всех нарисованных объектов
    ListHoldObjects listHO;
    //Таймер увеличения круга
    QTimer *timerPress;

    //Текущий увеличиваемый объект
    HoldObject currentObject;

    //Пишет лог на главную форму
    void writeLog(QString what, QString message);
};

#endif // PAINTBOX_H
