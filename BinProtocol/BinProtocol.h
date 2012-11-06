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

//����������� ���������� �������
#define BINPROT_DLE (0xf1)
#define BINPROT_EXT (0xf2)
#define BINPROT_DXT (0xf3)

//������� ��������������
//<DLE> = <DLE><DLE>
//<EXT> = <DLE><DXT>
//<DXT> = <DXT>

//����������� �� ������ ���� � ����� - ������ ���� � 2 ���� ������ ������� ������, � �� ����� ���� ������ 32000
extern int BINPROT_MAX_SIZE_OF_RECIEVED_PACKET;

//�� ��������� ����������� SMALL, ���� ����� BIG - ���� ���������� BINPROT_BIG_PACKET
//#define BINPROT_BIG_PACKET
//SMALL: <DLE><Header><data><crc><EXT>
//BIG:   <EXT><DLE><Header><data><crc><DLE><EXT>

struct TBP_Header{
	char Type;           	//��� ������ (� ����� ������ �������� �����)
    unsigned char Serial; 	//�������� ����� ���������� (255-Broadcast)
};

typedef struct{
	char Type;           	//��� ������ (� ����� ������ �������� �����)
    unsigned char Serial; 	//�������� ����� ���������� (255-Broadcast)
    unsigned char Data[1];	//�������� ������
}TBP_Pack;

typedef void (FGlobalSendChar)(unsigned char c);

//������� ��������� �������� �����, ��� ������ � ������� ����� ����� � 255 ����� ��������������,
//���� BP_MY_SERIAL=0 �� ����������� ��� ������.
//������������� ���������������� ����� ������� BP_Init_Protocol
extern unsigned char BP_MY_SERIAL;

//���������� ������� � �������� ������������� ������, ���������, ���������� ������� - ���������� true ���� ��� ��������� � ����� � false ���� ���
//�������� ���������������� �������. �� ����� ����� ��� ������ �������, �������� � ���������� �������
bool BP_Init_Protocol(void *Buf,unsigned short Len,unsigned char MySerial,FGlobalSendChar *GlobalSendChar);

//���������� ������� � �������� ������������� ������, ���������, ���������� ������� - ���������� true ���� ��� ��������� � ����� � false ���� ���
//�� ����� �������� �����, ��������, � ����� ��� �������
//������ ����������������� ������ ���������� ������� ����� ������������ ������ � ��������� �����.
//��������� ������ �� ������ ����� ����� BP_GetAndClearInternalBufferSize() � BP_InternalBuf
bool BP_Init_Protocol_InternalBuffer(void *Buf_receive,unsigned short Len_rec,unsigned char MySerial,void *Buf_send,unsigned short Len_send);

//������ ���������� ��� ������ ����� ���-�� �����
void BP_ReceiveChar(unsigned char c);

// true ���� � �������� ������ ���� ����� �����
bool BP_IsPackReceived(void);



//����������� ������ � ������� ���
//� ������ ������������� �������������� CRC
void BP_SendPack(char Type,unsigned char Serial,const void *Data,unsigned short Len);

//Serial=BP_MY_SERIAL ����� ����������� ������ � ������� ���
//� ������ ������������� �������������� CRC
void BP_SendMyPack(char Type,const void *Data,unsigned short Len);


//����� ��������, ��� ���������� ������ �� ���������� � ������ �� �������� ����
//���� ������������ ����������� ARM - ����� ��� ���������� ������ ��� ������������ ������

//��� ��������� �� ����� (�� �� ������� ���)
//Len = 2+data_length ��� Packet.  �.�. ������ ��� ������ len=2
//� LEN �� ������ CRC
//���������� NULL ���� ������ ������ ��� � ����� �� ������ ������ (����� <DLE> � ��� ����������� ��������), Len=����� �����
//���� ������ ��������� ���� NULL - ����� �������� :)
const TBP_Pack* BP_GetPacket(unsigned short *Len);

//��� ��������� �� ������ ������, ��������� ��� � �������� ����� ���������� (�� �� ������� ���)
//Len = data_length ��� Data
//� LEN �� ������ CRC
//���������� NULL ���� ������ ������ ���
//���� ������ ��������� ���� NULL - ����� �������� :)
const void * BP_GetData(char *Type, unsigned char *Serial, unsigned short *Len);

//�������� ����� � ����� ����������, � ������� ��� �� �������
//���������� NULL ���� ������ ������ ���
//�� ����� Len ���������� ����� ����������� ��� ����� �����, ���� ������ - ����� ���������,
//���� = 0 - ����������� ����������� ��������� ������ ������.
//���� ������ ��������� ���� NULL - ����� �������� :)
TBP_Pack* BP_ExtractPacket(void *Mesto,unsigned short *Len);

//�������� ������ ������ � ����� ����������, ��������� ��� � �������� ����� ���������� � ������� ��� �� �������
//���������� NULL ���� ������ ������ ���
//�� ����� Len ���������� ����� ����������� ��� ����� �����, ���� ������ - ����� ���������,
//���� = 0 - ����������� ����������� ��������� ������ ������.
//���� ������ ��������� ���� NULL - ����� �������� :)
void* BP_ExtractData(char *Type, unsigned char *Serial,void *Mesto,unsigned short *Len);


//������� �������� ����� ��������� ������ ����� ������� BP_GetPacket, BP_GetData
void BP_DropOnePacket(void);

//�������� �����
//� ������� �� ��� ��� ����
void BP_ClearBuffer(void);

//���� ������ ��������� ������ �� ������ �������� � ������������ �������� ������� � ���� ������
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
