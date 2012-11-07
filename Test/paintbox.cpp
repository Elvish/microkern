#include <QPainter>
#include <QTime>
#include "paintbox.h"
#include "PackTypes.h"
#include "../BinProtocol/BinProtocol.h"

PaintBox::PaintBox(QWidget *parent) :
    QWidget(parent)
{
    qsrand(QTime(0,0,0).secsTo(QTime::currentTime()));
    timerPress = NULL;
    currentColor = Qt::white;
    currentObject.radius = 0;
    currentObject.color = Qt::white;

    QTimer *tim = new QTimer();
    connect(tim,SIGNAL(timeout()),this,SLOT(timerCheckPacks()));
    tim->start(50);
}

extern MainWindow *LinkToSendClass;
void PaintBox::writeLog(QString what, QString message)
{
    LinkToSendClass->writeLog(what,message);
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

void PaintBox::drawOneObject(QPainter *qp, PaintBox::HoldObject &obj)
{
    //QPen pen.setColor(o.color);
    QBrush brush(obj.color,Qt::SolidPattern);
    //qp->setPen(pen);
    qp->setBrush(brush);
    qp->drawEllipse(obj.x-obj.radius,obj.y-obj.radius,obj.radius*2,obj.radius*2);
}


void PaintBox::drawLines(QPainter *qp)
{
    //QPen pen(Qt::black, 1, Qt::SolidLine);
    //QBrush brush(Qt::SolidPattern);
    //qp->setPen(pen);
    //qp->setBrush(brush);
    foreach(HoldObject obj, listHO){
        drawOneObject(qp,obj);
    }
    if(currentObject.radius>0)drawOneObject(qp,currentObject);


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
        currentObject.radius = 0;
        //currentColor = getColorByXY(event->x(),event->y());
        currentObject.color = getColorByXY(event->x(),event->y());
        TPackColor packColor;
        //packColor.color
        QColor c = getColorByXY(event->x(),event->y());
        packColor.r = c.redF();
        packColor.g = c.greenF();
        packColor.b = c.blueF();
        //c.getRgbF(packColor.r,packColor.g,packColor.b);
        BP_SendMyPack('C',&packColor,sizeof(packColor));
        return;
    }

    //return;


    currentObject.radius = 1;
    currentObject.x = event->x();
    currentObject.y = event->y();
    //currentObject.color = currentColor;
    //listHO.append(o);
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
    if(currentObject.radius>0){
        listHO.append(currentObject);
        TPackDraw packDraw;
        packDraw.x = currentObject.x;
        packDraw.y = currentObject.y;
        packDraw.radius = currentObject.radius;
        BP_SendMyPack('d',&packDraw,sizeof(packDraw));
    }
}



void PaintBox::timerWhenPressed()
{
    if(currentObject.radius<1)return;
    if(currentObject.radius++ > 50)if(timerPress){delete timerPress; timerPress = NULL;}
    update();
}

void PaintBox::timerCheckPacks()
{
    while(BP_IsPackReceived()){
        writeLog("Receive","Pack Received");
        char packType;
        unsigned char packSerial;
        TPackAllTypes pack;
        unsigned short dataSize = sizeof(pack);

        if(!BP_ExtractData(&packType,&packSerial,&pack,&dataSize)){
            writeLog("Receive","ERROR ExtractPack");
            return;
        }

        switch(packType){
        case 'C':
            if(dataSize!=sizeof(pack.color)) {
                writeLog("Receive",QString("ERROR get Color pack, but different size. Should=%1, but %2").arg(sizeof(pack.color)).arg(dataSize));
                return;
            }
            currentObject.color = QColor::fromRgbF(pack.color.r,pack.color.g,pack.color.b);

            writeLog("Receive",QString("Receive color: %1").arg(currentObject.color.value()));
            break;
        case 'd':{
            if(dataSize!=sizeof(pack.draw)) {
                writeLog("Receive",QString("ERROR get Draw pack, but different size. Should=%1, but %2").arg(sizeof(pack.draw)).arg(dataSize));
                return;
            }
            HoldObject obj;
            obj.color = currentObject.color;
            obj.x = pack.draw.x;
            obj.y = pack.draw.y;
            obj.radius = pack.draw.radius;
            listHO.append(obj);
            update();
            writeLog("Receive",QString("Receive object: (%1,%2),%3").arg(obj.x).arg(obj.y).arg(obj.radius));
            break;
        }
        default:
            writeLog("Receive","ERROR get unknown packtype");
        }
    }
}


