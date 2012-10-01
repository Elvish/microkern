#include <string.h>
#include "BinProtocol.h"

//#define NULL 0//((void*)0)

//#define DEBUG2
//#define DEBUG
//#define SUPERDEBUG
//#define DEBUG_SEND

//#define DEBUGCRITICAL
//#define DEBUG_OTLADKA    //для синхронизации с отсыльщиком пакетов

#ifndef DEBUGCRITICAL							
#ifdef DEBUG2
#define DEBUGCRITICAL
#endif
#ifdef DEBUG
#define DEBUGCRITICAL
#endif
#ifdef SUPERDEBUG
#define DEBUGCRITICAL
#endif
#endif


void log_printfi(char *c);
#ifdef DEBUG
#define log_printf(m,c) log_printfi(c)
#else
#define log_printf(m,c)
#endif

/*#undef log_printf(c)
#include "VP_logger.h"
#undef log_printf2
#undef log_printfc
#undef log_printfi
//#define log_printf_std log_printf
//#define log_printf(c)
*/

#ifdef DEBUGCRITICAL
#include <stdio.h>
#define log_printfc(c) log_printfi(c)
#else
#define log_printfc(c)
#endif

#ifdef DEBUG2
#define log_printf2(c) log_printfi(c)
#else
#define log_printf2(c)
#endif

#ifdef DEBUG
char DEBUG_buffer[1024];
#include <stdio.h>
#endif

#ifdef DEBUG2
char DEBUG_buffer2[1024];
#include <stdio.h>
#endif



typedef unsigned int TColor ;
void c_DebugPrint_LogC(TColor col,const char *format,...);
#define clRed (63488)


int BINPROT_MAX_SIZE_OF_RECIEVED_PACKET=245;

 unsigned char* PreBuffer=NULL;
 unsigned char* NormalBuffer=NULL;
 unsigned short BufLen=0;
//Информация хранится следующим образом:
//
// ....<Len_2><Header_2><Data....><Len_2><Header_2><Data....>....
//     ^-BeginPtr                                            ^-EndPtr
//
// Буфер поделен на две части:
// PreLoadPack - size = BINPROT_MAX_SIZE_OF_RECIEVED_PACKET;
// NormalPacks - size = BufLen-BINPROT_MAX_SIZE_OF_RECIEVED_PACKET;

unsigned char BP_MY_SERIAL=0;	// принимаются все пакеты...


 unsigned char* BeginPtr=NULL;
 unsigned char* EndPtr=NULL;

//Необходимо вызвать в качестве инициализации буфера - возвращает true если все параметры в норме и false если нет
static bool BP_SetReceiveBuffer(void *Buf,unsigned short Len,unsigned char MySerial)
{
	//if(MySerial == 255)return false;
	//если MySerial == 255 прием только broadcast пакетов
	if(Len>2000)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET = 900;
	if(Len<3000)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET = 900;
	if(Len<2000)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET = 600;
	if(Len<1500)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET = 450;
	if(Len<1000)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET = 270;
	if(Len< 500)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET = 100;
	if(Len< 200)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET =  50;
	if(Len< 100)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET =  20;
	if(Len<  50)BINPROT_MAX_SIZE_OF_RECIEVED_PACKET =  10;
	BeginPtr = EndPtr = NormalBuffer;
	if(Len < BINPROT_MAX_SIZE_OF_RECIEVED_PACKET * 2 - 4)return false;
	PreBuffer = Buf;
	NormalBuffer = (unsigned char*)Buf + BINPROT_MAX_SIZE_OF_RECIEVED_PACKET + 2;
	BufLen = Len - BINPROT_MAX_SIZE_OF_RECIEVED_PACKET - 2;
	BP_MY_SERIAL = MySerial;
	BeginPtr = EndPtr = NormalBuffer;
	log_printf("BINPROTO","Normal Init");
	return true;
}
static FGlobalSendChar *GlobalSendChar;
bool BP_Init_Protocol(void *Buf,unsigned short Len,unsigned char MySerial,FGlobalSendChar *GlSendChar)
{
	bool res = BP_SetReceiveBuffer(Buf,Len,MySerial);
	if(!GlSendChar)res=false;
	GlobalSendChar = GlSendChar;
	//c_DebugPrint_LogC(clRed,"BP_Init_Protocol: Buf=%p len=%d, serial=%d, SendChar=%p",Buf,Len,MySerial,GlSendChar);
	return res;
}
static int InternalBuffSize=0;
static int InternalBufPos=0;
unsigned char *BP_InternalBuf=NULL;
static void InternalSendChar(unsigned char c)
{
	if(InternalBufPos<InternalBuffSize)BP_InternalBuf[InternalBufPos++] = c;
}
int BP_GetAndClearInternalBufferSize(void)
{
	int size = InternalBufPos;
	InternalBufPos = 0;
	return size;
}

bool BP_Init_Protocol_InternalBuffer(void *Buf_receive,unsigned short Len_rec,unsigned char MySerial,void *Buf_send,unsigned short Len_send)
{
	BP_Init_Protocol(Buf_receive,Len_rec,MySerial,InternalSendChar);
	InternalBufPos = 0;
	InternalBuffSize = Len_send;
	BP_InternalBuf = Buf_send;
	return true;
}
#define TestSpecialSimvol(c) (c==BINPROT_DLE || c==BINPROT_EXT)
#define ROR(c) {c=(c>>1) | ((c&0x0001)?0x8000:0);}
#define ROL(c) {c=(c<<1) | ((c&0x8000)?0x0001:0);}
#define RORB(c) {c=(c>>1) | ((c&0x01)?0x80:0);}
#define ROLB(c) {c=(c<<1) | ((c&0x80)?0x01:0);}




// mode 1
//#define CRC_VARS 	unsigned char CRC_xor=0
//#define DoCRC(c) {CRC_xor^=c;}
//#define DoCRCFinal (CRC_xor)

//SEPER mode 2
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor++;CRC_add+=(c)^CRC_xor;}
//#define DoCRCFinal (CRC_xor^((CRC_add>>8)&0xff)^((CRC_add)&0xff))

//mode 3
//#define CRC_VARS 	unsigned short CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {ROL(CRC_xor);CRC_xor^= c;}
//#define DoCRCFinal (((CRC_xor>>8)&0xff)^((CRC_xor)&0xff))

//Лажа: mode 4
//#define CRC_VARS 	unsigned short CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {ROL(CRC_xor);CRC_xor^= c;}
//#define DoCRCFinal (0)

// mode 5
//#define CRC_VARS 	unsigned short CRC_xor=0;unsigned short CRC_add=0;unsigned char CRC_pos=0
//#define DoCRC(c) {ROL(CRC_xor);CRC_xor^= c ^ (++CRC_pos);}
//#define DoCRCFinal (((CRC_xor>>8)&0xff)^((CRC_xor)&0xff))


//SUPER mode 6
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor++;CRC_add=(CRC_add>>1)|((CRC_add&1)?0x8000:0);CRC_add+=((c))^CRC_xor;}
//#define DoCRCFinal (CRC_xor^((CRC_add>>8)&0xff)^((CRC_add)&0xff))


//mode 7
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor^=c;CRC_add+=c;}
//#define DoCRCFinal (CRC_xor^((CRC_add>>8)&0xff)^((CRC_add)&0xff))

//mode 8 (from mode 2)
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor++;CRC_add+=(c)^CRC_xor;}
//#define DoCRCFinal (((CRC_add>>8)&0xff)^((CRC_add)&0xff))

//mode 9 (from mode 8) good
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_tmp;unsigned char CRC_add=0
//#define DoCRC(c) {CRC_xor++;CRC_tmp = CRC_add + (c)^CRC_xor; CRC_add = (CRC_tmp&0xff);if(CRC_tmp>0xff)CRC_add++,CRC_tmp-=0x100;}
//#define DoCRCFinal (CRC_add)

//mode 10 (from mode 9)
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor++;CRC_add += (c)^CRC_xor; if(CRC_add>0xff)CRC_add-=0xff;}
//#define DoCRCFinal (CRC_add&0xff)

//mode 11 (from mode 9)
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_tmp;unsigned char CRC_add=0
//#define DoCRC(c) {CRC_xor=0;CRC_tmp = CRC_add + (c)^CRC_xor; CRC_add = (CRC_tmp&0xff);if(CRC_tmp>0xff)CRC_add++,CRC_tmp-=0x100;}
//#define DoCRCFinal (CRC_add)

//mode 12 (from mode 9)
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_tmp;unsigned char CRC_add=0
//#define DoCRC(c) {CRC_xor=CRC_add;CRC_tmp = CRC_add + (c)^CRC_xor; CRC_add = (CRC_tmp&0xff);if(CRC_tmp>0xff)CRC_add++,CRC_tmp-=0x100;}
//#define DoCRCFinal (CRC_add)

//mode 13 (from mode 9) good
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_tmp;unsigned char CRC_add=0
//#define DoCRC(c) {CRC_xor++;ROLB(CRC_add);CRC_tmp = CRC_add + (c)^CRC_xor; CRC_add = (CRC_tmp&0xff);if(CRC_tmp>0xff)CRC_add++,CRC_tmp-=0x100;}
//#define DoCRCFinal (CRC_add)

//mode 14 (from mode 9+13)
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor++;ROL(CRC_add);CRC_add += (c)^CRC_xor; if(CRC_add>0xff)CRC_add-=0xff;}
//#define DoCRCFinal (CRC_add&0xff)

//mode 15 (from mode 10)
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor++;CRC_add += (c)^CRC_xor; if(CRC_add>0xff)CRC_add-=0x123;}
//#define DoCRCFinal (CRC_add&0xff)

//mode 16
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned short CRC_add=0
//#define DoCRC(c) {CRC_xor++;ROLB(CRC_add);CRC_add ^= (c)^CRC_xor; if(CRC_add>0xff)CRC_add-=0xff;}
//#define DoCRCFinal (CRC_add&0xff)

//mode 17
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned char CRC_add=0
//#define DoCRC(c) {CRC_xor++; CRC_add = (((c)^CRC_xor) + CRC_add)%19; }
//#define DoCRCFinal (CRC_add)

//mode 18
//#define CRC_VARS 	unsigned char CRC_xor=0;unsigned char CRC_add=0
//#define DoCRC(c) {CRC_xor++; ROLB(CRC_add); CRC_add ^= (((c)^CRC_xor))%227; }
//#define DoCRCFinal (CRC_add)

//mode 19 (from mode 13) good  best
#define CRC_VARS 	unsigned char CRC_xor=0xAA;unsigned short CRC_tmp;unsigned char CRC_add=0x0f
#define DoCRC(c) {CRC_xor++;CRC_tmp = ((((unsigned short)CRC_add)<<1) + (c))^CRC_xor; if(CRC_tmp&0x100)CRC_tmp++; CRC_add=CRC_tmp;}
#define DoCRCFinal (CRC_add)

//mode 20 (from mode 19)
//#define CRC_VARS 	unsigned char CRC_xor=0xAA;unsigned short CRC_tmp=0xf0;unsigned char CRC_add=0x0f
//#define DoCRC(c) {CRC_xor++;CRC_tmp += (CRC_add<<1) + (c)^CRC_xor; if(CRC_tmp&0x100)CRC_tmp++; CRC_add=CRC_tmp;}
//#define DoCRCFinal (CRC_add)

//mode 21 (from mode 20)
//#define CRC_VARS 	unsigned char CRC_xor=0xAA;unsigned char CRC_add=0x0f
//#define DoCRC(c) {CRC_xor++;CRC_add += (CRC_add<<1) + (c)^CRC_xor; if(CRC_add&0x80)CRC_add++; }
//#define DoCRCFinal (CRC_add)

//mode 22 (from mode 20)
//#define CRC_VARS 	unsigned char CRC_xor=0xAA;unsigned char CRC_add=0x0f
//#define DoCRC(c) {CRC_xor++;CRC_add += (CRC_add<<2) ^ (c)^CRC_xor; }
//#define DoCRCFinal (CRC_add)

#ifdef DEBUG_SEND
#define DoCRCsend(c) {DoCRC(c);PrintCRCSendDEBUG(c,CRC_xor,CRC_add);}
void PrintCRCSendDEBUG(unsigned char byte,unsigned short CRC_xor,unsigned short CRC_add)
{
	char DEBUG_buffer[1024];
	sprintf(DEBUG_buffer,"SEND CRC: byte=%u, xor=%u, add=%u, [%u]",byte,CRC_xor,CRC_add,DoCRCFinal);
	log_printfc(DEBUG_buffer);
}
#else
#define DoCRCsend(c) DoCRC(c)
#endif
void BP_SendPack(char Type,unsigned char Serial,const void *VData,unsigned short Len)
{
	CRC_VARS;
	const unsigned char *Data=VData;
	//c_DebugPrint_LogC(clRed,"SendPack: Type=%c, Serial=%d, Len=%d",Type,Serial,Len);
	//c_DebugPrint_LogC(clRed,"SendPack: sndchar=%p",GlobalSendChar);
	if(TestSpecialSimvol((unsigned char)Type) || TestSpecialSimvol(Serial)){//не посылаем заведомо неисправный пакет
		log_printfc("Bad Sending Packet - ignore it!");
		#ifdef DEBUGCRITICAL
		{
			char buf[1024];
			sprintf(buf,"BadPacket: Type=%u, Serial=%u, DLE=%u, EXT=%u",(unsigned char)Type,Serial,BINPROT_DLE,BINPROT_EXT);
			log_printfc(buf);
		}
		#endif
/*   		log_printf("BINPROTO","ERROR PACK: Type=%d, Serial=%d, first=%s, second=%s, full=%s",Type,Serial,
			(TestSpecialSimvol((unsigned char)Type))?"true":"false",
			(TestSpecialSimvol((unsigned char)Serial))?"true":"false",
			(TestSpecialSimvol((unsigned char)Type) || TestSpecialSimvol(Serial))?"true":"false");     //  */
		;;
		;;
		Type=Type;
		Serial=Serial;
		;;
		;;
		return;
	}

	//c_DebugPrint_LogC(clRed,"SendPack: Step1");

	#ifdef BINPROT_BIG_PACKET
	GlobalSendChar(BINPROT_EXT);
	//c_DebugPrint_LogC(clRed,"SendPack: Step1a");
	#endif
	GlobalSendChar(BINPROT_DLE);
	//c_DebugPrint_LogC(clRed,"SendPack: Step1b");
	GlobalSendChar(Type);
	GlobalSendChar(Serial);

	DoCRCsend((unsigned char)Type);
	DoCRCsend(Serial);

	//c_DebugPrint_LogC(clRed,"SendPack: Step2");

	//for(;Len>=0;Len--){
	for(;Len<65533;Len--){
		unsigned char sym;
		if(Len){sym = *Data++;DoCRCsend(sym);}
		else sym=DoCRCFinal;
		switch(sym){
		  case BINPROT_DLE:GlobalSendChar(BINPROT_DLE);GlobalSendChar(BINPROT_DLE);break;
		  case BINPROT_EXT:GlobalSendChar(BINPROT_DLE);GlobalSendChar(BINPROT_DXT);break;
		  default:GlobalSendChar(sym);
		}
	}

	//c_DebugPrint_LogC(clRed,"SendPack: Step3");

	#ifdef BINPROT_BIG_PACKET
	GlobalSendChar(BINPROT_DLE);
	#endif
	GlobalSendChar(BINPROT_EXT);

	//c_DebugPrint_LogC(clRed,"SendPack: Step4");
}



//Serial=BP_MY_SERIAL затем переработка пакета и отсылка его через GlobalSendChar
void BP_SendMyPack(char Type,const void *Data,unsigned short Len)
{
	BP_SendPack(Type,BP_MY_SERIAL,Data,Len);
}


static bool IsReceivingPack=false;
#ifdef BINPROT_BIG_PACKET
static bool IsReceivedEXT=false;
#endif
static unsigned char *PrePtr=NULL;

static bool LastSimvolDLE=false;
static bool IgnoreThisPacket=false;

#ifdef DEBUG_OTLADKA
unsigned char LastCRC;

bool WAIT_SEND=false;
#define WAITSEND {WAIT_SEND=true;}
#else
#define WAITSEND
#endif


static void WriteShort(unsigned char *ptr,unsigned short value)
{
 	*ptr++ = value&0xff;
	*ptr = (value>>8)&0xff;
	
}

static unsigned short ReadShort(const unsigned char *ptr)
{
 	unsigned char val1,val2;
	val1 = *ptr++;
	val2 = *ptr;
	return val1 + (val2<<8);
}


static void TestPacket(void)
{
	unsigned short Len;
	CRC_VARS;
	unsigned char *kk;
	unsigned char *EndTempPtr=EndPtr;
	//struct TBP_Header * header = ((struct TBP_Header*)PreBuffer);
	unsigned char pack_serial = ((struct TBP_Header*)PreBuffer)->Serial;
	//unsigned char pack_serial = header->Serial;
	PrePtr--;//отсекаем CRC;
	if(pack_serial != 255 && BP_MY_SERIAL && BP_MY_SERIAL!=pack_serial)return;
	log_printf("BINPROTO","TEST - Normal Serial");

	for(kk = PreBuffer;kk < PrePtr;kk++){
		DoCRC(*kk);
		#ifdef DEBUG
		sprintf(DEBUG_buffer,"CRC: byte=%u, xor=%u, add=%u, [%u]",*kk,CRC_xor,CRC_add,DoCRCFinal);
		log_printf(DEBUG_buffer);
		#endif
	}

	#ifdef DEBUG
	sprintf(DEBUG_buffer,"TEST - CRC: podshet=%d, real=%d",DoCRCFinal,PrePtr[0]);
	log_printf(DEBUG_buffer);
	sprintf(DEBUG_buffer,"TEST - CRC: [-1]=%d, [0]=%d, [1]=%d)",PrePtr[-1],PrePtr[0],PrePtr[1]);
	log_printf(DEBUG_buffer);
	#endif


	if(PrePtr < PreBuffer){//Bad Packet (нет данных вообще)
		log_printfc("Bad PrePtr");
		return;
	}

	if(*PrePtr != DoCRCFinal){
		log_printf("BINPROTO","Bad CRC!");
		log_printf2("Bad CRC!");
		return;
	}
	#ifdef DEBUGCRITICAL
	LastCRC=*PrePtr;
	{char buf[1024];
	sprintf(buf,"Get CRC=%d, Rec CRC=%d",DoCRCFinal,PrePtr[0]);
	log_printf(buf);}
	#endif
	log_printf("BINPROTO","TEST - Normal CRC");

	// пакет подходит по всем параметрам, переносим его в нашу очередь
	Len = PrePtr - PreBuffer;			//Длина пакета вместе с заголовком
	#ifdef DEBUG2
	sprintf(DEBUG_buffer2,"INSERTB Begin=%d, End=%d, Len=%d",BeginPtr-NormalBuffer,EndPtr-NormalBuffer,Len);
	log_printf2(DEBUG_buffer2);
	#endif
	if(Len<2){
		log_printf2("TEST1 - short packet");
		log_printf("BINPROTO","TEST2 - short packet");
		return;
	}
	if(EndPtr < BeginPtr && EndPtr + Len + 2 + 1  >= BeginPtr ){
		log_printfc("TEST - BUFFER OVERFLOW BEFORE RECYCLE!!!");
		WAITSEND;
		return;//Net mesta
	}
	if(BufLen - (EndPtr - NormalBuffer) < Len + 2 + 1 + 4){	//Нет места до конца буфера 4-метка рецикла буфера
		//*(unsigned short*)EndPtr = 0xffff;	//указание что до конца буфера больше нет пакетов
		WriteShort(EndPtr,0xffff);
		EndTempPtr=NormalBuffer;
		log_printf2("TEST - recycle buffer");
		log_printf("BINPROTO","TEST - recycle buffer");
		if(BeginPtr - EndTempPtr < Len + 2 + 1) {
			log_printfc("TEST - BUFFER OVERFLOW FROM RECYCLE!!!");
			WAITSEND;
			return;//не помещается
		}
	}
	log_printf("BINPROTO","TEST - place packet into buffer");
	//*(unsigned short*)EndTempPtr = Len;
	WriteShort(EndTempPtr,Len);
	memcpy(EndTempPtr + 2,PreBuffer,Len);
	EndPtr = EndTempPtr + 2 + Len;

	#ifdef DEBUG2
	sprintf(DEBUG_buffer2,"INSERTA Begin=%d, End=%d",BeginPtr-NormalBuffer,EndPtr-NormalBuffer);
	log_printf2(DEBUG_buffer2);
	#endif


}


//Должна вызываться при приеме байта где-то извне
void BP_ReceiveChar(unsigned char c)
{
	if(!IsReceivingPack){
		switch(c){
		  case BINPROT_DLE:
			log_printf("BINPROTO","BEGIN - DLE");
			#ifdef BINPROT_BIG_PACKET
			if(!IsReceivedEXT)return;//Ignore rubbish
			IsReceivedEXT=false;
			#endif
			IsReceivingPack=true;
			PrePtr=PreBuffer;
			//CRC_xor=CRC_add=0;
			LastSimvolDLE=false;
			IgnoreThisPacket=false;
			return;
		  case BINPROT_EXT:
			log_printf("BINPROTO","BEGIN - EXT");
			#ifndef BINPROT_BIG_PACKET
			return;
			#else
			IsReceivedEXT = true;
			return;
			#endif
		  default:
			log_printf("BINPROTO","BEGIN - other simvol...");
			return;
		}
	}
	//Режим приема заголовка и данных
	if(c == BINPROT_EXT){//Конец пакета
		log_printf("BINPROTO","END - EXT");
		#ifdef BINPROT_BIG_PACKET
		if(!LastSimvolDLE){
			IsReceivingPack = false;
			return;
		}
		#endif
		IsReceivingPack = false;
		if(IgnoreThisPacket)return;
		//Пакет принят, проверяем CRC, SERIAL и т.п.
		log_printf("BINPROTO","END - before test");
		TestPacket();

		return;
	}

	if(PrePtr - PreBuffer > BINPROT_MAX_SIZE_OF_RECIEVED_PACKET){
		IgnoreThisPacket=true;//BufferOverflow
		return;
	}

	if(LastSimvolDLE){
		LastSimvolDLE = false;
		switch(c){
		  case BINPROT_DLE:
			log_printf("BINPROTO","LAST_DLE - DLE");
			*PrePtr++ = BINPROT_DLE;
			return;
		  case BINPROT_DXT:
			log_printf("BINPROTO","LAST_DLE - DXT");
			*PrePtr++ = BINPROT_EXT;
			return;
		  //case BINPROT_EXT://Never this case!
		  default://нарушение протокола
			log_printf("BINPROTO","LAST_DLE - bad simvol!");
			IsReceivingPack=false;
			return;
		}
	}

	if(c == BINPROT_DLE){
		log_printf("BINPROTO","NORM - DLE");
		LastSimvolDLE = true;
		return;
	}

	#ifdef SUPERDEBUG
	{
	char DEBUG_buffer[1024];
	sprintf(DEBUG_buffer,"NORM - sym = %u",c);
	log_printf(DEBUG_buffer);
	}
	#endif
	*PrePtr++ = c;

}

// NULL если целого пакета нет и адрес на начало пакета (после <DLE>), Len=общей длине
const TBP_Pack* BP_GetPacket(unsigned short *Len)
{
	const TBP_Pack* ret;
	//c_DebugPrint_LogC(clRed,"GetPack: LenPtr=%p BP=%p, EP=%p, NB=%p",Len,BeginPtr,EndPtr,NormalBuffer);
	if(BeginPtr == EndPtr){
		//c_DebugPrint_LogC(clRed,"GetPack: return NULL");
		return NULL;
	}
	if((*Len = ReadShort(BeginPtr)) == 0xffff){
		*Len = ReadShort(NormalBuffer);
		ret = (const TBP_Pack*)(NormalBuffer+2);
		//c_DebugPrint_LogC(clRed,"GetPack: return1 %p, Type=%c Len=%d S=%d",ret,ret->Type,Len,ret->Serial);
		return ret;
	}
	ret = (const TBP_Pack*)(BeginPtr+2);
	//c_DebugPrint_LogC(clRed,"GetPack: return2 %p, Type=%c Len=%d S=%d",ret,ret->Type,Len,ret->Serial);
	return ret;
}

//аналогично предыдущему дает указатель на данные
const void * BP_GetData(char *Type, unsigned char *Serial, unsigned short *Len)
{
	const TBP_Pack *p=BP_GetPacket(Len);
	if(!p){
		*Type=0;
		*Serial=0;
		*Len=0;
		return NULL;
	}
	if(Type)*Type=p->Type;
	if(Serial)*Serial=p->Serial;
	*Len-=2;
	return p->Data;
}

//копирует пакет в место назначения, и удаляет его из очереди
TBP_Pack* BP_ExtractPacket(void *Mesto,unsigned short *Len)
{
	unsigned short PackLen,CLen,PLen;
	const TBP_Pack *p=BP_GetPacket(&PackLen);
	if(!p){
		*Len=0;
		return NULL;
	}
	//PackLen+=sizeof(struct TBP_Header);
	if(!Len)PLen=BINPROT_MAX_SIZE_OF_RECIEVED_PACKET;else PLen=*Len;
	CLen = (PackLen > PLen)?PLen:PackLen;
	memcpy(Mesto,p,CLen);
	//*Len=PackLen-sizeof(struct TBP_Header);
	*Len=PackLen;
	BP_DropOnePacket();
	return Mesto;
}

//аналогично предыдущему дает указатель на данные
void* BP_ExtractData(char *Type, unsigned char *Serial,void *Mesto,unsigned short *Len)
{
	unsigned short PackLen,CLen,PLen;
	const TBP_Pack *p=BP_GetPacket(&PackLen);

	#ifdef DEBUG2
	sprintf(DEBUG_buffer2,"PackLen=%d, <ptr=%p>",PackLen,p);
	log_printf2(DEBUG_buffer2);
	#endif


	if(!p){
		*Type=0;
		*Serial=0;
		*Len=0;
		return NULL;
	}
	if(!Len)PLen=BINPROT_MAX_SIZE_OF_RECIEVED_PACKET;else PLen=*Len;
	CLen = (PackLen > PLen)?PLen:PackLen;
	memcpy(Mesto,p->Data,CLen);
	*Len = PackLen - sizeof(struct TBP_Header);
	if(Type)*Type = p->Type;
	if(Serial)*Serial = p->Serial;
	BP_DropOnePacket();
	return Mesto;
}




//общее
void BP_DropOnePacket(void)	//следует вызывать после обработки пакета после функций BP_GetPacket, BP_GetData
{
	unsigned short Len=ReadShort(BeginPtr);

	#ifdef DEBUG2
	sprintf(DEBUG_buffer2,"BEFORE Begin=%d, End=%d, Len=%d",BeginPtr-NormalBuffer,EndPtr-NormalBuffer,Len);
	log_printf2(DEBUG_buffer2);
	#endif

	if(BeginPtr==EndPtr)return;
	if(Len==0xffff){
		BeginPtr = NormalBuffer + ReadShort(NormalBuffer) + 2;
	}else{
		BeginPtr += Len + 2;
	}

	#ifdef DEBUG2
	sprintf(DEBUG_buffer2,"AFTER  Begin=%d, End=%d, Len=%d",BeginPtr-NormalBuffer,EndPtr-NormalBuffer,Len);
	log_printf2(DEBUG_buffer2);
	#endif

}

void BP_ClearBuffer(void)	//зануляет буфер
{
	BeginPtr = EndPtr;
}

bool BP_IsPackReceived(void)
{
	return (BeginPtr != EndPtr)?true:false;
}


