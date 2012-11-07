#ifndef PACKTYPES_H
#define PACKTYPES_H


#include "../BinProtocol/Types.h"

struct TPackDraw{
    int x;
    int y;
    int radius;
};


struct TPackColor{
    float r,g,b;
};


union TPackAllTypes{
    TPackDraw draw;
    TPackColor color;
    unsigned char Raw[12];
};


#endif // PACKTYPES_H
