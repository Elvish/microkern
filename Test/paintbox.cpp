#include <QPainter>
#include <QTime>
#include "paintbox.h"

PaintBox::PaintBox(QWidget *parent) :
    QWidget(parent)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    timerPress = NULL;
    currentColor = Qt::white;
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
  QBrush brush(Qt::black,Qt::SolidPattern);
  qp.fillRect(e->rect(),brush);
  drawLines(&qp);
  drawPalitra(&qp);
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

void PaintBox::drawPalitra(QPainter *qp)
{
    QPen pen(Qt::black, 1, Qt::SolidLine);
    for(int x=0;x<width();x++)for(int y=0;y<20;y++){
        pen.setColor(getColorByXY(x,y));
        qp->setPen(pen);
        qp->drawLine(x,y,x+1,y);
        //qp->drawPixmap();
    }
}

static int holm(int p,int br)
{
    if(p<=0 || p>=256*2)return br;
    if(p<256)return (p + br < 256) ? p + br : 255;
    return (511 - p < 256) ? 511 - p : 255;
}

QColor PaintBox::getColorByXY(int x, int y)
{
    int l = width();
    int peace = x*(255*4)/l;
    int br = y*200/20;
    return QColor(holm(peace%(256*3),br),holm(peace-256,br),holm((peace-512+256*3)%(256*3),br));
}

void PaintBox::mousePressEvent(QMouseEvent *event)
{
    if(event->y() < 20){
        currentColor = getColorByXY(event->x(),event->y());
        return;
    }
    HoldObject o;
    o.radius = 1;
    o.x = event->x();
    o.y = event->y();
    o.color = currentColor;
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
