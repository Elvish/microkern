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
 *****************************************/

#ifdef __cplusplus
extern "C" {
#endif

#include "..\Kobus712\hardware\src\include\types.h"

#define BINPROT_DLE (0xf1)
#define BINPROT_EXT (0xf2)
#define BINPROT_DXT (0xf3)

//правила преобразования
//<DLE> = <DLE><DLE>
//<EXT> = <DLE><DXT>
//<DXT> = <DXT>

//ограничение на случай шума в линии - должно быть в 2 раза меньше размера буфера, и не может быть больше 32000
/*#ifdef BAZA_SOFT
#define BINPROT_MAX_SIZE_OF_RECIEVED_PACKET (2048)
#else
#define BINPROT_MAX_SIZE_OF_RECIEVED_PACKET (245)
#endif  */

extern int BINPROT_MAX_SIZE_OF_RECIEVED_PACKET;

//#define BINPROT_BIG_PACKET
//SMALL <DLE><Header><data><EXT>
//BIG   <EXT><DLE><Header><data><DLE><EXT>

struct TBP_Header{
	char Type;           	//тип пакета (в общем случае условная буква)
	unsigned char Serial; 	//серийный номер КОБУС (255-Broadcast)
};

typedef struct{
	char Type;           	//тип пакета (в общем случае условная буква)
	unsigned char Serial; 	//серийный номер КОБУС (255-Broadcast)
	unsigned char Data[1];	//условнае данные
}TBP_Pack;

//следует присвоить серийный номер, все пакеты кроме этого и 255 будут игнорироваться,
//если BP_MY_SERIAL=0 то принимаются все пакеты...
//автоматически инициализируется через функцию BP_Init_Protocol
extern unsigned char BP_MY_SERIAL;

//переработка пакета и отсылка его
void BP_SendPack(char Type,unsigned char Serial,const void *Data,unsigned short Len);
//Serial=BP_MY_SERIAL затем переработка пакета и отсылка его
void BP_SendMyPack(char Type,const void *Data,unsigned short Len);
//к данным автоматически присоединяется CRC

typedef void (FGlobalSendChar)(unsigned char c);
//Необходимо вызвать в качестве инициализации буфера, серийника, отсылающей функции - возвращает true если все параметры в норме и false если нет
bool BP_Init_Protocol(void *Buf,unsigned short Len,unsigned char MySerial,FGlobalSendChar *GlobalSendChar);
//Необходимо вызвать в качестве инициализации буфера, серийника, отсылающей функции - возвращает true если все параметры в норме и false если нет
bool BP_Init_Protocol_InternalBuffer(void *Buf_receive,unsigned short Len_rec,unsigned char MySerial,void *Buf_send,unsigned short Len_send);
//Должна вызываться при приеме байта где-то извне
void BP_ReceiveChar(unsigned char c);
// true если в буфере есть целый пакет
bool BP_IsPackReceived(void);

//Len = 2+data_length для Packet.  Т.е. пакета без данных len=2
//Len = data_length для Data
const TBP_Pack* BP_GetPacket(unsigned short *Len);// NULL если целого пакета нет и адрес на начало пакета (после <DLE>), Len=общей длине
const void * BP_GetData(char *Type, unsigned char *Serial, unsigned short *Len);//аналогично предыдущему дает указатель на данные
TBP_Pack* BP_ExtractPacket(void *Mesto,unsigned short *Len);//копирует пакет в место назначения, и удаляет его из очереди
void* BP_ExtractData(char *Type, unsigned char *Serial,void *Mesto,unsigned short *Len);//аналогично предыдущему дает указатель на данные
//В LEN не входит CRC

//общее
void BP_DropOnePacket(void);	//следует вызывать после обработки пакета после функций BP_GetPacket, BP_GetData
void BP_ClearBuffer(void);	//зануляет буфер

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
