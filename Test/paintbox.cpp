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
    currentObject.radius = 0;
    currentObject.color = Qt::white;

    //Раз в 50мс будем проверять вновь пришедшие пакеты.
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
  QPainter qp(this);
  QBrush brush(Qt::black,Qt::SolidPattern);
  //Обновляем и рисуем только на обновляемой области, для производительности
  qp.fillRect(e->rect(),brush);
  drawObjects(&qp,e->rect());
  //Если обновление надо сделать вверху - перерисовываем всю палитру сразу
  if(e->rect().top()<25)drawPalitra(&qp);
}

void PaintBox::drawOneObject(QPainter *qp, PaintBox::HoldObject &obj)
{
    QBrush brush(obj.color,Qt::SolidPattern);
    qp->setBrush(brush);
    qp->drawEllipse(obj.x-obj.radius,obj.y-obj.radius,obj.radius*2,obj.radius*2);
}

//Отрисовка конкретного объекта на Painter
void PaintBox::drawObjects(QPainter *qp, const QRect &rect)
{
    foreach(HoldObject obj, listHO){
        //Рисуем только если объект попал в зону рисования
        int r = obj.radius+2;
        QRect zona = rect;
        zona.adjust(-r,-r,r,r);
        if(zona.contains(obj.x,obj.y)){
            //obj.color = qrand();
            drawOneObject(qp,obj);
        }
    }
    //Если в данный момент у нас идет работа с текущим объектом - рисуем его
    if(currentObject.radius>0)drawOneObject(qp,currentObject);


}

//Рисуем палитру
void PaintBox::drawPalitra(QPainter *qp)
{
    QPen pen(Qt::black, 1, Qt::SolidLine);
    for(int x=0;x<width();x++)for(int y=0;y<20;y++){
        pen.setColor(getColorByXY(x,y));
        qp->setPen(pen);
        qp->drawLine(x,y,x+1,y);
    }
}

//Преобразование компоненты цвета
//в виде графика __/\__
//и добавление к этой компоненте яркости
//с проверкой на границы компоненты 0..255
static int holm(int p,int br)
{
    if(p<=0 || p>=256*2)return br;
    if(p<256)return (p + br < 256) ? p + br : 255;
    return (511 - p + br < 256) ? 511 - p + br : 255;
}

//Дает цвет палитры в зависимости от координат
//По цветам:
// R: ./\_/.
// G: ._/\_.
// B: .\_/\.
// И с добавкой яркости по смещению к низу
QColor PaintBox::getColorByXY(int x, int y)
{
    int l = width();
    int peace = x*(255*4)/l;
    int br = y*255/20;
    return QColor(holm(peace%(256*3),br),holm(peace-256,br),holm((peace-512+256*3)%(256*3),br));
}

//Чистим все объекты. Если до этого что-то было - то отсылаем пакет на чистку соседа
void PaintBox::ClearAll(bool isSendPack)
{
    if(listHO.count() || currentObject.radius>0){
        listHO.clear();
        currentObject.radius = 0;
        update();
    }
    //Команда на очистку не требует параметров
    if(isSendPack)BP_SendMyPack('Z',NULL,0);
}


void PaintBox::mousePressEvent(QMouseEvent *event)
{
    //Если клацнули вверху -значит попали в палитру
    if(event->y() < 20){
        currentObject.radius = 0;
        //Получаем цвет палитры
        currentObject.color = getColorByXY(event->x(),event->y());
        //Пакет с данными по цветовым компонентам
        TPackColor packColor;
        //Заполняем пакет компоненами цвета
        packColor.r = currentObject.color.redF();
        packColor.g = currentObject.color.greenF();
        packColor.b = currentObject.color.blueF();
        //Отсылаем пакет с указанием что это цветовой пакет
        BP_SendMyPack('C',&packColor,sizeof(packColor));
        return;
    }

    //Клацнули на основную зону
    //Рисуем кружок, начиная с маленького размерчика
    currentObject.radius = 1;
    currentObject.x = event->x();
    currentObject.y = event->y();
    updateNearObject(currentObject);

    //Устанавливаем таймер увеличения кружка
    if(timerPress)delete timerPress;
    timerPress = new QTimer();
    connect(timerPress,SIGNAL(timeout()),this, SLOT(timerWhenPressed()));
    timerPress->start(20);
}

void PaintBox::mouseReleaseEvent(QMouseEvent *event)
{
    //Отпустили мышку.
    //Выключаем таймер
    if(timerPress)delete timerPress;
    timerPress = NULL;
    //Если до этого мы клацали в основной зоне и расшиперивали кружок
    if(currentObject.radius>0){
        //Добавлем в список законченных объектов
        listHO.append(currentObject);

        TPackDraw packDraw;
        //Заполняем пакет с данным о кружке, но БЕЗ цвета
        packDraw.x = currentObject.x;
        packDraw.y = currentObject.y;
        packDraw.radius = currentObject.radius;
        //Отправлем "кружок" соседям.
        BP_SendMyPack('d',&packDraw,sizeof(packDraw));

        //объявить готовность к следующим клацаньям
        currentObject.radius = 0;
    }
}

void PaintBox::updateNearObject(PaintBox::HoldObject &obj)
{
    update(obj.x - obj.radius - 10,obj.y - obj.radius - 10,obj.radius*2 + 20,obj.radius*2 + 20);
}


//Увеличиваем радиус кружка на каждый тик
void PaintBox::timerWhenPressed()
{
    if(currentObject.radius<1)return;
    //Если слишком большой - прекратить расшиперку
    if(currentObject.radius++ > 50)if(timerPress){delete timerPress; timerPress = NULL;}
    updateNearObject(currentObject);
    //Здесь мы НЕ отправляем соседу кружок, так как все равно будет отпущена кнопка мышки
}

//Цикл обработки пакетов от соседа
//Вызывает по событию таймера
void PaintBox::timerCheckPacks()
{
    //Пока есть пакеты их обрабатываем и не выходим из цикла
    while(BP_IsPackReceived()){
        writeLog("Receive","Pack Received");
        //Тип пакета
        char packType;
        //От кого пришёл.
        unsigned char packSerial;
        //Универсальное хранилище для всех типов пакетов
        TPackAllTypes pack;
        //Максимальный допустимый размер извлекаемого пакета.
        //извлеченный пакет может быть и меньшего размера
        unsigned short dataSize = sizeof(pack);

        if(!BP_ExtractData(&packType,&packSerial,&pack,&dataSize)){
            writeLog("Receive","ERROR ExtractPack");
            return;
        }

        //Пакет извлечен и ожидает обработки.
        //Удобнее всего обрабатывать по свичу
        switch(packType){
        case 'C'://Пакет с цветом
            //Проверяем что получен пакет правильного размера,
            if(dataSize!=sizeof(pack.color)) {
                writeLog("Receive",QString("ERROR get Color pack, but different size. Should=%1, but %2").arg(sizeof(pack.color)).arg(dataSize));
                return;
            }
            //Воссоздаем цвет из данных пакета
            currentObject.color = QColor::fromRgbF(pack.color.r,pack.color.g,pack.color.b);

            writeLog("Receive",QString("Receive color: %1").arg(currentObject.color.value()));
            break;
        case 'd':{//Пакет с объектом
            //Проверяем что получен пакет правильного размера,
            if(dataSize!=sizeof(pack.draw)) {
                writeLog("Receive",QString("ERROR get Draw pack, but different size. Should=%1, but %2").arg(sizeof(pack.draw)).arg(dataSize));
                return;
            }
            //Извлекаем кружок из пакета
            HoldObject obj;
            obj.x = pack.draw.x;
            obj.y = pack.draw.y;
            obj.radius = pack.draw.radius;

            //Дополняем этот кружок текущим цветом
            obj.color = currentObject.color;

            //добавляем его в объекты и отрисовываем всё
            listHO.append(obj);
            updateNearObject(obj);
            writeLog("Receive",QString("Receive object: (%1,%2),%3").arg(obj.x).arg(obj.y).arg(obj.radius));
            break;
        }
        case 'Z'://Пакет с командой очистки
            //Не проверяем длину пакета, так как нам не важно, что в данных, есть ли они вообще.
            writeLog("Receive",QString("Receive CLEAR command"));
            //Чистим. Так как мы чистим по команде от соседа,
            //то запрещаем отсылать к соседу команду очистки
            ClearAll(false);
            break;
        default://Неизвестный пакет
            writeLog("Receive","ERROR get unknown packtype");
        }
    }
}


