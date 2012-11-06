#ifndef _BINPROTOCOL_H_
#define _BINPROTOCOL_H_

/****************************************
 * This Code writted by Kern Elvish (c) 2005
 * Bin Protocol 0.2
 * modified by Kern at 2008.02.08 in Brazil.
 * Взаимодействие бинарными пакетами переменной длины
 *
 * Инициализация:
 *  static char buffer_for_bp[1024];
 *  BP_SetReceiveBuffer(buffer_for_bp,sizeof(buffer_for_bp),SOME_SERIAL);
 * ---- также надо определить функцию
 * extern "C" void GlobalSendChar(char c){ComPort->WriteByte(c);}
 * ---- а в функции приема байта вызывать эту функцию:
 * extern "C" void BP_ReceiveChar(char c);
 *
 *
** Copyright (C) Kern Elvish.
** All rights reserved.
** Contact: Kern Elvish (KernElvish@yandex.ru)
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Kern Elvish.
**
**
**
 *
 *****************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "types.h"

//Управляющие протоколом символы
#define BINPROT_DLE (0xf1)
#define BINPROT_EXT (0xf2)
#define BINPROT_DXT (0xf3)

//правила преобразования
//<DLE> = <DLE><DLE>
//<EXT> = <DLE><DXT>
//<DXT> = <DXT>

//ограничение на случай шума в линии - должно быть в 2 раза меньше размера буфера, и не может быть больше 32000
extern int BINPROT_MAX_SIZE_OF_RECIEVED_PACKET;

//По умолчанию принимается SMALL, если нужен BIG - надо определить BINPROT_BIG_PACKET
//#define BINPROT_BIG_PACKET
//SMALL: <DLE><Header><data><crc><EXT>
//BIG:   <EXT><DLE><Header><data><crc><DLE><EXT>

struct TBP_Header{
	char Type;           	//тип пакета (в общем случае условная буква)
    unsigned char Serial; 	//серийный номер устройства (255-Broadcast)
};

typedef struct{
	char Type;           	//тип пакета (в общем случае условная буква)
    unsigned char Serial; 	//серийный номер устройства (255-Broadcast)
    unsigned char Data[1];	//условные данные
}TBP_Pack;

typedef void (FGlobalSendChar)(unsigned char c);

//следует присвоить серийный номер, все пакеты с адресом кроме этого и 255 будут игнорироваться,
//если BP_MY_SERIAL=0 то принимаются все пакеты.
//автоматически инициализируется через функцию BP_Init_Protocol
extern unsigned char BP_MY_SERIAL;

//Необходимо вызвать в качестве инициализации буфера, серийника, отсылающей функции - возвращает true если все параметры в норме и false если нет
//основная инициализирующая функция. На входе буфер для приема пакетов, серийник и отсылающая функция
bool BP_Init_Protocol(void *Buf,unsigned short Len,unsigned char MySerial,FGlobalSendChar *GlobalSendChar);

//Необходимо вызвать в качестве инициализации буфера, серийника, отсылающей функции - возвращает true если все параметры в норме и false если нет
//На входе применый буфер, серийник, и буфер для отсылки
//Вместо непосредственного вызова отсылающей функции пишет получившиеся данные в указанный буфер.
//Извлекать данные из буфера можно через BP_GetAndClearInternalBufferSize() и BP_InternalBuf
bool BP_Init_Protocol_InternalBuffer(void *Buf_receive,unsigned short Len_rec,unsigned char MySerial,void *Buf_send,unsigned short Len_send);

//Должна вызываться при приеме байта где-то извне
void BP_ReceiveChar(unsigned char c);

// true если в приемном буфере есть целый пакет
bool BP_IsPackReceived(void);



//переработка пакета и отсылка его
//к данным автоматически присоединяется CRC
void BP_SendPack(char Type,unsigned char Serial,const void *Data,unsigned short Len);

//Serial=BP_MY_SERIAL затем переработка пакета и отсылка его
//к данным автоматически присоединяется CRC
void BP_SendMyPack(char Type,const void *Data,unsigned short Len);


//Важно отметить, что получаемые пакеты НЕ ВЫРОВНЕННЫ в памяти по границам слов
//Если используется архитектура ARM - нужен или побайтовый доступ или коипирование данных

//Даёт указатель на пакет (но не удаляет его)
//Len = 2+data_length для Packet.  Т.е. пакета без данных len=2
//В LEN не входит CRC
//возвращает NULL если целого пакета нет и адрес на начало пакета (после <DLE> и без управляющих символов), Len=общей длине
//Если вместо указателй дать NULL - будут проблемы :)
const TBP_Pack* BP_GetPacket(unsigned short *Len);

//Даёт указатель на данные пакета, заполняет тип и серийник через переменные (но не удаляет его)
//Len = data_length для Data
//В LEN не входит CRC
//возвращает NULL если целого пакета нет
//Если вместо указателй дать NULL - будут проблемы :)
const void * BP_GetData(char *Type, unsigned char *Serial, unsigned short *Len);

//копирует пакет в место назначения, и удаляет его из очереди
//возвращает NULL если целого пакета нет
//на входе Len должнабыть равна выделяемому под пакет месту, если меньше - пакет усекается,
//если = 0 - принимается максимально возможный размер апкета.
//Если вместо указателй дать NULL - будут проблемы :)
TBP_Pack* BP_ExtractPacket(void *Mesto,unsigned short *Len);

//копирует данные пакета в место назначения, заполняет тип и серийник через переменные и удаляет его из очереди
//возвращает NULL если целого пакета нет
//на входе Len должнабыть равна выделяемому под пакет месту, если меньше - пакет усекается,
//если = 0 - принимается максимально возможный размер апкета.
//Если вместо указателй дать NULL - будут проблемы :)
void* BP_ExtractData(char *Type, unsigned char *Serial,void *Mesto,unsigned short *Len);


//следует вызывать после обработки пакета после функций BP_GetPacket, BP_GetData
void BP_DropOnePacket(void);

//зануляет буфер
//и удаляет всё что там есть
void BP_ClearBuffer(void);

//Дает размер ликвидных данных из буфера отправки и одновременно обнуляет позицию в этом буфере
int BP_GetAndClearInternalBufferSize(void);
extern unsigned char *BP_InternalBuf;

//CRC считается из header и data
//в среде распространения пакетов в данных есть хотябы один байт - CRC :)
//при BINPROT_BIG_PACKET минимальная длина пакета 7 байт, из них 5 обслуживающих канальный уровень байт
//при!BINPROT_BIG_PACKET минимальная длина пакета 5 байт, из них 3 обслуживающих канальный уровень байт
//
//если нет места в очереди для записи пакета - игнорируются новые приходящие пакеты


#ifdef __cplusplus
}
#endif

#endif
