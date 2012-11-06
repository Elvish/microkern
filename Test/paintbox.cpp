#include <QPainter>
#include <QTime>
#include "paintbox.h"

PaintBox::PaintBox(QWidget *parent) :
    QWidget(parent)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    timerPress = NULL;
}

void PaintBox::timerWhenPressed()
{
    if(listHO.count()<1)return;
    if(listHO.last().radius++ > 50)if(timerPress){delete timerPress; timerPress = NULL;}
    update();
}


void PaintBox::paintEvent(QPaintEvent *e)
{
  Q_UNUSED(e);
  QPainter qp(this);
  drawLines(&qp);
}

void PaintBox::drawLines(QPainter *qp)
{
  QPen pen(Qt::black, 1, Qt::SolidLine);
  QBrush brush(Qt::SolidPattern);
  qp->setPen(pen);
  qp->setBrush(brush);
  foreach(HoldObject o, listHO){
      pen.setColor(o.color);
      brush.setColor(o.color);
      qp->setPen(pen);
      qp->setBrush(brush);
      //qp->drawRect(o.x-o.radius,o.y+o.radius,o.radius*2,o.radius*2);
      qp->drawEllipse(o.x-o.radius,o.y-o.radius,o.radius*2,o.radius*2);
  }


}

void PaintBox::mousePressEvent(QMouseEvent *event)
{
    HoldObject o;
    o.radius = 1;
    o.x = event->x();
    o.y = event->y();
    o.color = QColor(qrand()%255,qrand()%255,qrand()%255);
    listHO.append(o);
    update();

    if(timerPress)delete timerPress;
    timerPress = new QTimer();
    connect(timerPress,SIGNAL(timeout()),this, SLOT(timerWhenPressed()));
    timerPress->start(20);
}

void PaintBox::mouseReleaseEvent(QMouseEvent *event)
{
    if(timerPress)delete timerPress;
    timerPress = NULL;
}
