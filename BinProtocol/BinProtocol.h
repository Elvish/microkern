#ifndef _BINPROTOCOL_H_
#define _BINPROTOCOL_H_

/****************************************
 * This Code writted by Kern Elvish (c) 2005
 * Bin Protocol 0.2
 * modified by Kern at 2008.02.08 in Brazil.
 * �������������� ��������� �������� ���������� �����
 *
 * �������������:
 *  static char buffer_for_bp[1024];
 *  BP_SetReceiveBuffer(buffer_for_bp,sizeof(buffer_for_bp),SOME_SERIAL);
 * ---- ����� ���� ���������� �������
 * extern "C" void GlobalSendChar(char c){ComPort->WriteByte(c);}
 * ---- � � ������� ������ ����� �������� ��� �������:
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

//������� ��������������
//<DLE> = <DLE><DLE>
//<EXT> = <DLE><DXT>
//<DXT> = <DXT>

//����������� �� ������ ���� � ����� - ������ ���� � 2 ���� ������ ������� ������, � �� ����� ���� ������ 32000
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
	char Type;           	//��� ������ (� ����� ������ �������� �����)
	unsigned char Serial; 	//�������� ����� ����� (255-Broadcast)
};

typedef struct{
	char Type;           	//��� ������ (� ����� ������ �������� �����)
	unsigned char Serial; 	//�������� ����� ����� (255-Broadcast)
	unsigned char Data[1];	//�������� ������
}TBP_Pack;

//������� ��������� �������� �����, ��� ������ ����� ����� � 255 ����� ��������������,
//���� BP_MY_SERIAL=0 �� ����������� ��� ������...
//������������� ���������������� ����� ������� BP_Init_Protocol
extern unsigned char BP_MY_SERIAL;

//����������� ������ � ������� ���
void BP_SendPack(char Type,unsigned char Serial,const void *Data,unsigned short Len);
//Serial=BP_MY_SERIAL ����� ����������� ������ � ������� ���
void BP_SendMyPack(char Type,const void *Data,unsigned short Len);
//� ������ ������������� �������������� CRC

typedef void (FGlobalSendChar)(unsigned char c);
//���������� ������� � �������� ������������� ������, ���������, ���������� ������� - ���������� true ���� ��� ��������� � ����� � false ���� ���
bool BP_Init_Protocol(void *Buf,unsigned short Len,unsigned char MySerial,FGlobalSendChar *GlobalSendChar);
//���������� ������� � �������� ������������� ������, ���������, ���������� ������� - ���������� true ���� ��� ��������� � ����� � false ���� ���
bool BP_Init_Protocol_InternalBuffer(void *Buf_receive,unsigned short Len_rec,unsigned char MySerial,void *Buf_send,unsigned short Len_send);
//������ ���������� ��� ������ ����� ���-�� �����
void BP_ReceiveChar(unsigned char c);
// true ���� � ������ ���� ����� �����
bool BP_IsPackReceived(void);

//Len = 2+data_length ��� Packet.  �.�. ������ ��� ������ len=2
//Len = data_length ��� Data
const TBP_Pack* BP_GetPacket(unsigned short *Len);// NULL ���� ������ ������ ��� � ����� �� ������ ������ (����� <DLE>), Len=����� �����
const void * BP_GetData(char *Type, unsigned char *Serial, unsigned short *Len);//���������� ����������� ���� ��������� �� ������
TBP_Pack* BP_ExtractPacket(void *Mesto,unsigned short *Len);//�������� ����� � ����� ����������, � ������� ��� �� �������
void* BP_ExtractData(char *Type, unsigned char *Serial,void *Mesto,unsigned short *Len);//���������� ����������� ���� ��������� �� ������
//� LEN �� ������ CRC

//�����
void BP_DropOnePacket(void);	//������� �������� ����� ��������� ������ ����� ������� BP_GetPacket, BP_GetData
void BP_ClearBuffer(void);	//�������� �����

int BP_GetAndClearInternalBufferSize(void);
extern unsigned char *BP_InternalBuf;

//CRC ��������� �� header � data
//� ����� ��������������� ������� � ������ ���� ������ ���� ���� - CRC :)
//��� BINPROT_BIG_PACKET ����������� ����� ������ 7 ����, �� ��� 5 ������������� ��������� ������� ����
//���!BINPROT_BIG_PACKET ����������� ����� ������ 5 ����, �� ��� 3 ������������� ��������� ������� ����
//
//���� ��� ����� � ������� ��� ������ ������ - ������������ ����� ���������� ������


#ifdef __cplusplus
}
#endif

#endif
