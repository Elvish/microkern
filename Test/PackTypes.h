#ifndef PACKTYPES_H
#define PACKTYPES_H


#include "../BinProtocol/Types.h"


//Структура для передачи координат и размера объекта
struct TPackDraw{
    int x;
    int y;
    int radius;
};


//Структура для передачи компонент цвета объекта
//Весьма избыточная, надо сказать... :)
struct TPackColor{
    float r,g,b;
};

//Каша из всех передаваемых по протоколу структур.
//Удобная точка их объединения и одновременная проверка
//на соответствие всех данных POD.
union TPackAllTypes{
    TPackDraw draw;
    TPackColor color;
    unsigned char Raw[12];
};


#endif // PACKTYPES_H
