////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2011, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:bte2prom.c
//
//		Description	:
//					BT Flash saving code
//
//		Changelog	:
//					yzhou@2013.10.16 initial version
///////////////////////////////////////////////////////////////////////////////
#include "type.h"
#include "app_config.h"
#include "os.h"
#include "spi_flash.h"
#include "nvm.h"
//#define SHOW_BT_PAIRING_INFO 

/***********************************************
64KB at least
***********************************************/
#ifndef DWNLDR_USRDAT_ADDR
#define DWNLDR_USRDAT_ADDR      (0x90)
#endif

#define MAX_BT_PAIRING_DATA_SIZE    (52*1024)
#define MAX_BT_ALIVE_ADDR_DATA_SIZE (8*1024)

//max is 64KB, Total size MUST be 4KB aligned.
#define ALIVE_ADDR_REC_SIZE  (9)


static uint32_t FlshGetAliveAddrWriteOffset(uint32_t StartOffset)
{
	uint32_t i;
	uint8_t Tmp[4];
	uint32_t Step = ALIVE_ADDR_REC_SIZE;//9 BYTE for alive addr (3 majic, 6 addr)
		
	for(i = StartOffset; i <= StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE  - Step ; i += Step)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			if(i != StartOffset)
			{
				return i - Step;
			}
			else
			{
				break; //data is full
			}
		}
		else if((Tmp[0] != 0xFF) || (Tmp[1] != 0xFF) || (Tmp[2] != 0xFF)) //some error data found, then skip it
		{
			if(i != StartOffset)
			{
				return i - Step;
			}
			else
			{
				break; //data is full
			}
		}
	}

	if(i > StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE - Step)
	{
		//empty data aera, for first usage
		return (StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE - (MAX_BT_ALIVE_ADDR_DATA_SIZE % Step) - Step);
	}

	//not found or buffer is full now
	SpiFlashErase(StartOffset, MAX_BT_ALIVE_ADDR_DATA_SIZE);
	return (StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE - (MAX_BT_ALIVE_ADDR_DATA_SIZE % Step) - Step);

}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
bool FlshGetAliveAddr(uint8_t* Addr)
{
	uint32_t i;
	uint8_t Tmp[4];
	uint32_t StartOffset, Step = ALIVE_ADDR_REC_SIZE; //9 BYTE for alive addr (3 majic, 6 addr)

//    SpiFlashInfoInit();

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&StartOffset, 4) < 0)
	{
		return 0;//error
	}

	StartOffset += MAX_BT_PAIRING_DATA_SIZE;

	for(i = StartOffset; i < StartOffset + MAX_BT_ALIVE_ADDR_DATA_SIZE / 9 * 9; i += Step)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			SpiFlashRead(i + 3, Addr, 6);
			return 1;
		}
		else if((Tmp[0] != 0xFF) || (Tmp[1] != 0xFF) || (Tmp[2] != 0xFF)) //some error data found, then skip it
		{
			break; //error data found, I just return fail.
		}
	}

	//not found or buufer is full now
	return 0;
}

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
bool FlshUpdateBtLatestDevAddr(uint8_t* BtAddr)
{
	uint32_t StartOffset, AliveOffset;
	uint32_t TmpOffset;
	uint8_t Tmp[4];

	//SpiFlashInfoInit();

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&StartOffset, 4) < 0)
	{
		return 0;//error
	}

	AliveOffset = StartOffset + MAX_BT_PAIRING_DATA_SIZE;
	TmpOffset = FlshGetAliveAddrWriteOffset(AliveOffset);
	DBG("Addr write Offset = %x\n", TmpOffset);
	if(TmpOffset >= AliveOffset)
	{
		Tmp[0] = 'O';
		Tmp[1] = '1';
		Tmp[2] = '8';
		SpiFlashWrite(TmpOffset, Tmp, 3);
		return (0 == SpiFlashWrite(TmpOffset + 3, BtAddr, 6)) ? 1 : 0;
	}

	return 0; //error
}

//---------------------------------------------------------------------------------
// 1 mean OK, 0 means error or do NOT use flash-saving function
static bool FlshGetBtPairingInfoOffsetCallBk(uint32_t* StartOffset, uint32_t* MaxSize)
{
	//SpiFlashInfoInit();

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)StartOffset, 4) != 0)
	{
		return 0;//error
	}

	*MaxSize = MAX_BT_PAIRING_DATA_SIZE;

	return  1;
}

static uint32_t FlshGetPairingInfoWriteOffset(uint32_t StartOffset, uint32_t Step)
{
	uint32_t i;
	uint8_t Tmp[4];

	for(i = StartOffset; i <= StartOffset + MAX_BT_PAIRING_DATA_SIZE - Step - 4/* 4 byte Magic Number */; i += Step)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			if(i != StartOffset)
			{
				return i - Step;
			}
			else
			{
				break; //"data is full, need to erase Flash
			}
		}
		else
		{
			if((Tmp[0] != 0xFF) || (Tmp[1] != 0xFF) || (Tmp[2] != 0xFF)) //some error data found, then skip it
			{
				if(i != StartOffset)
				{
					return i - Step;
				}
				else
				{
					break; //data is full
				}
			}
		}
	}
	
	if( i > StartOffset + MAX_BT_PAIRING_DATA_SIZE - Step -4) /* 4 means 4 byte Magic Number */
	{
		//empty DATA aera, for first usage
		return (StartOffset + MAX_BT_PAIRING_DATA_SIZE - 4 - ((MAX_BT_PAIRING_DATA_SIZE - 4) % Step) - Step);
	}
		

	//erase & write magic number
	SpiFlashErase(StartOffset, MAX_BT_PAIRING_DATA_SIZE);
	Tmp[0] = 'P';
	Tmp[1] = 'R';
	Tmp[2] = 'I';
	Tmp[3] = 'F';
	SpiFlashWrite(StartOffset + MAX_BT_PAIRING_DATA_SIZE - 4, Tmp, 4);	//write magic number
	return (StartOffset + MAX_BT_PAIRING_DATA_SIZE - 4 - ((MAX_BT_PAIRING_DATA_SIZE - 4) % Step) - Step);

}



//write the magic number "PRIF" at the end of PAIRING DATA BANK
//call this funtcion before using BT
bool FlshBTInfoAreaInit(void)
{
	uint32_t UserDataAddrBegin;
	uint8_t Tmp[4];
	
	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&UserDataAddrBegin, 4) != 0)
	{
		return FALSE;
	}
	
	SpiFlashRead(UserDataAddrBegin + MAX_BT_PAIRING_DATA_SIZE - 4, Tmp, 4);
	if((Tmp[0] == 'P') && (Tmp[1] == 'R') && (Tmp[2] == 'I' )&& (Tmp[3] == 'F'))
	{
		//initialized
		return TRUE;
	}
	else
	{
		//for first initialization, erase Flash and write magic number
		//erase all PAIRING info bank and alive address bank 52K+8K
		SpiFlashErase(UserDataAddrBegin, MAX_BT_PAIRING_DATA_SIZE + MAX_BT_ALIVE_ADDR_DATA_SIZE);
		
		Tmp[0] = 'P';
		Tmp[1] = 'R';
		Tmp[2] = 'I';
		Tmp[3] = 'F';
		SpiFlashWrite(UserDataAddrBegin + MAX_BT_PAIRING_DATA_SIZE - 4, Tmp, 4);	//write magic number
		return TRUE;
		
	}
	
}	


//save nvram info to flash, bt callback function
bool FlshUpDateBtPairingInfoCallBk(uint32_t NvRamOffset, uint32_t Step/*full record block step size*/, uint32_t NvRamDataSize/**/)
{
	uint32_t StartOffset, t;
	uint8_t Tmp[4];

	if(NvRamDataSize + 3 > Step)
	{
		return 0; //error, MUST including 3 majic number
	}

	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return 0;
	}

	StartOffset = FlshGetPairingInfoWriteOffset(StartOffset, Step);

	DBG("-----------------------------------0x%x\n", StartOffset);
	Tmp[0] = 'O';
	Tmp[1] = '1';
	Tmp[2] = '8';
	SpiFlashWrite(StartOffset, Tmp, 3);
	StartOffset += 3;
	while(NvRamDataSize)
	{
		if(NvRamDataSize > 4)
		{
			t = 4;
		}
		else
		{
			t = NvRamDataSize;
		}

		NvmRead(NvRamOffset, Tmp, t);

		SpiFlashWrite(StartOffset, Tmp, t);

		StartOffset += t;
		NvRamDataSize -= t;
		NvRamOffset += t;
	}

	return 1;
}

//load flash info to nvram if any.
bool FlshLoadBtPairingInfoCallBk(uint32_t NvRamOffset, uint32_t RecStep)
{
	uint32_t StartOffset, t, i, j;
	uint8_t Tmp[4];

	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return 0;
	}

	Tmp[0] = 0;
	NvmWrite(NvRamOffset, Tmp, 1); //save init default value

	for(i = StartOffset; i <= StartOffset + MAX_BT_PAIRING_DATA_SIZE - RecStep; i += RecStep)
	{
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			SpiFlashRead(i + 3, Tmp, 1); //rec count

			NvmWrite(NvRamOffset, Tmp, 1); //count

			t = (uint32_t)(Tmp[0]) * 23;
			i += 4;
			NvRamOffset++;

			while(t)
			{
				if(t >= 4)
				{
					j = 4;
				}
				else
				{
					j = t;
				}

				SpiFlashRead(i, Tmp, j);
				NvmWrite(NvRamOffset, Tmp, j);
				NvRamOffset += j;
				t -= j;
				i += j;
			}
			return 1;
		}
	}

	return 0;
}

#ifdef SHOW_BT_PAIRING_INFO
static void ShowFlashLoadResult(int RecIdxFromZero, unsigned char *BtAddr,unsigned char *LinkKey,unsigned char *Property);
#endif
//save up to 8 bt device rec method
//up to 8 device supported
#if 1
//读取一个设备的配对信息，返回1表示加载成功
typedef int(*FuncGet1of8RecInfo)(uint8_t RecIdx/*from 0*/, uint8_t* Data/*23B*/);
int FlshLoad1of8Dev(int OneFullRecBlockSize, int RecIdxFromZero, uint8_t* BtAddr, uint8_t* LinkKey, uint8_t* Property)
{
	uint8_t Tmp[3];
	uint32_t i, StartOffset;

	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &i))
	{
		return 0;
	}
    
    OneFullRecBlockSize += 3;//including 3Bytes of sync data,zhouyi,20140411

	//step 2 get the data
	for(i = StartOffset; i <= StartOffset + MAX_BT_PAIRING_DATA_SIZE - OneFullRecBlockSize; i += OneFullRecBlockSize)
	{
//		OSRescheduleTimeout(2);
		SpiFlashRead(i, Tmp, 3);
		if((Tmp[0] == 'O') && (Tmp[1] == '1') && (Tmp[2] == '8'))
		{
			SpiFlashRead(i + 3, Tmp, 1); //rec count

			if(RecIdxFromZero >= (uint32_t)Tmp[0])
			{
				return 0;//not found
			}

			i = i + RecIdxFromZero * 23 + 4;

			//get addr
			SpiFlashRead(i, BtAddr, 6);

			//key
			SpiFlashRead(i + 6, LinkKey, 16);

			//property
			SpiFlashRead(i + 22, Property, 1);

#ifdef SHOW_BT_PAIRING_INFO            
            ShowFlashLoadResult(RecIdxFromZero, BtAddr,LinkKey,Property);
#endif            
			return 1;
		}
	}

	return 0;
}

static uint8_t OneBtRec[23];

#ifdef SHOW_BT_PAIRING_INFO
static void ShowOneDevSavedPairingInfo(int RecId,int AddrOffset, int Size);
static void ShowOneDevMemoryPairedInfo(int RecId,unsigned char* Info, int Size);
#endif

void FlashSave8DevRec(int OneFullRecBlockSize, int TotalRecNum, FuncGet1of8RecInfo GetOneInf)
{
	uint8_t i, Tmp[3];
	uint32_t t, StartOffset;
	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return;
	}
    
    OneFullRecBlockSize += 3;//including 3Bytes of sync data,zhouyi,20140411

	StartOffset = FlshGetPairingInfoWriteOffset(StartOffset, OneFullRecBlockSize);
	DBG("-----------------------------------New0x%x\n", StartOffset);
	Tmp[0] = 'O';
	Tmp[1] = '1';
	Tmp[2] = '8';
	SpiFlashWrite(StartOffset, Tmp, 3);
	StartOffset += 3;

	//total num
	Tmp[0] = TotalRecNum & 0xFF;
	SpiFlashWrite(StartOffset, Tmp, 1);
	StartOffset++;
	for(i = 0; i < TotalRecNum; i++)
	{
		//get data
		if(GetOneInf(i, OneBtRec))
		{
#ifdef SHOW_BT_PAIRING_INFO            
			ShowOneDevMemoryPairedInfo(i,OneBtRec,23);
#endif            
			SpiFlashWrite(StartOffset, OneBtRec, 23);
			StartOffset += 23;
#ifdef SHOW_BT_PAIRING_INFO            
            ShowOneDevSavedPairingInfo(i,StartOffset - 23,23);
#endif            
		}
	}
}

#ifdef SHOW_BT_PAIRING_INFO
//~~~~~~~~~~~~~~~~~~
static void ShowOneDevMemoryPairedInfo(int RecId,unsigned char* Info, int Size)
{
    int i,t;
    DBG("MemID = %d: ",RecId);
    for(i = 0 ; i < Size ; i ++)
    {
        t = Info[i];
        t &= 0xFF;
        DBG("%x ",t);
    }
    DBG("\n");
}
static void ShowOneDevSavedPairingInfo(int RecId,int AddrOffset, int Size)
{
    int i,t;
    uint8_t C;
    DBG("RecID = %d: ",RecId);
    for(i = 0; i < Size ; i ++)
    {
        SpiFlashRead(AddrOffset + i, &C, 1);
        t = C; 
        t &= 0xFF;
        DBG("%x ",t);
    }
    DBG("\n");
}
//#endif //#ifdef SHOW_BT_PAIRING_INFO
//#if 1//def SHOW_BT_PAIRING_INFO
static void ShowFlashLoadResult(int RecIdxFromZero, unsigned char *BtAddr,unsigned char *LinkKey,unsigned char *Property)
{
    int i,t;
    DBG("FlhID = %d: ",RecIdxFromZero);
    for(i = 0 ; i < 6 ; i ++)
    {
        t = BtAddr[i]; 
        t &= 0xFF;
        DBG("%x ",t);
    }
    
    for(i = 0 ; i < 16 ; i ++)
    {
        t = LinkKey[i]; 
        t &= 0xFF;
        DBG("%x ",t);
    }
    
    t = Property[0]; 
        t &= 0xFF;
        DBG("%x\n",t);
}
#endif //show bt pairing info

#endif //# If 1,支持8个设置，都保存在Flash中


/*
Format:
"BTUDIF"+6B"bdaddr"+N"length"+"Name"
*/

/* Name format:  @#.......@

*/
#define BTUDIF_INFO_ERROR  (0xFFFFFFFF)
uint32_t CheckValidatedBTUDIF(uint8_t* UniCode16/*BE*/, uint32_t Len)
{
	uint32_t i, Pos = 0;

	if(Len < 8)
	{
		return BTUDIF_INFO_ERROR;
	}

	//get start code
	for(i = 0; i < Len - 2; i += 2)
	{
		if(UniCode16[i] != 0x00)
		{
			continue;
		}
		if(UniCode16[i + 1] != '@')
		{
			continue;
		}

		//check next sig: #
		if(i < Len - 4)
		{
			if(UniCode16[i + 2] != 0x00)
			{
				continue;
			}
			if(UniCode16[i + 3] != '#')
			{
				continue;
			}
			//found "@#"
			Pos = i + 4;
			break;
		}
	}

	//check next sig: "#"
	if(Len - i < 4)
	{
		return BTUDIF_INFO_ERROR;
	}
	for(i = Pos; i < Len; i += 2)
	{
		if(UniCode16[i] != 0x00)
		{
			continue;
		}
		if(UniCode16[i + 1] != '@')
		{
			continue;
		}

		return Pos;
	}

	return BTUDIF_INFO_ERROR;
}
#define BTUDIN_OFFSET   (MAX_BT_PAIRING_DATA_SIZE + MAX_BT_ALIVE_ADDR_DATA_SIZE)
#define MAX_UDIN_NAME_LEN  (50)
//UniCode2Utf8
void NewBTDevInfoUpdate(uint8_t* UniCode16/*BE*/, uint32_t Len, uint8_t* NewBdAddr)
{
	uint32_t t, StartOffset, Size;
	uint8_t Addr[6] = "\0";

	//step 0 check the syntax
	StartOffset = CheckValidatedBTUDIF(UniCode16, Len);
	if(BTUDIF_INFO_ERROR == StartOffset)//not found
	{
		return;
	}
	UniCode16 = UniCode16 + StartOffset;
	Len = Len - StartOffset;

	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return;
	}

	SpiFlashRead(StartOffset + BTUDIN_OFFSET, Addr, 6); //Sync data
	if((Addr[0] != 'B') || (Addr[1] != 'T') ||
	        (Addr[2] != 'U') || (Addr[3] != 'D') ||
	        (Addr[4] != 'I') || (Addr[5] != 'F')) //first time
	{
		//erase all
		SpiFlashErase(StartOffset + BTUDIN_OFFSET, 4096);

		//sync data
		Addr[0] = 'B';
		Addr[1] = 'T';
		Addr[2] = 'U';
		Addr[3] = 'D';
		Addr[4] = 'I';
		Addr[5] = 'F';
		SpiFlashWrite(StartOffset + BTUDIN_OFFSET, Addr, 6); //sync data

		//new bdaddr if any
		if(NewBdAddr)
		{
			Addr[0] = NewBdAddr[0];
			Addr[1] = NewBdAddr[1];
			Addr[2] = NewBdAddr[2];
			Addr[3] = NewBdAddr[3];
			Addr[4] = NewBdAddr[4];
			Addr[5] = NewBdAddr[5];
		}
		else
		{
			Addr[0] = '\0';
			Addr[1] = '\0';
			Addr[2] = '\0';
			Addr[3] = '\0';
			Addr[4] = '\0';
			Addr[5] = '\0';
		}
		SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 6, Addr, 6); //default bdaddr,zero
	}
	else
	{
		//Save Old BdAddr
		if(!NewBdAddr)
		{
			SpiFlashRead(StartOffset + BTUDIN_OFFSET + 6, Addr, 6);
		}
		else
		{
			Addr[0] = NewBdAddr[0];
			Addr[1] = NewBdAddr[1];
			Addr[2] = NewBdAddr[2];
			Addr[3] = NewBdAddr[3];
			Addr[4] = NewBdAddr[4];
			Addr[5] = NewBdAddr[5];
		}

		//erase all
		SpiFlashErase(StartOffset + BTUDIN_OFFSET, 4096);

		SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 6, Addr, 6); //restore bdaddr

		//sync data
		Addr[0] = 'B';
		Addr[1] = 'T';
		Addr[2] = 'U';
		Addr[3] = 'D';
		Addr[4] = 'I';
		Addr[5] = 'F';
		SpiFlashWrite(StartOffset + BTUDIN_OFFSET, Addr, 6); //sync data
	}

	//get and save info
	Size = 0;
	while(Len >= 2)
	{
		t = *UniCode16++;
		t <<= 8;
		t |= *UniCode16++;

		Len -= 2;

		if(Size > MAX_UDIN_NAME_LEN - 4) //max MAX_UDIN_NAME_LEN bytes
		{
			Addr[0] = 0;
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 1);
			Size++;
			break;
		}
		//check sig #
		if(t == '@')//end of name
		{
			Addr[0] = 0;
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 1);
			Size++;
			break;
		}

		if(t <= 0x7F)
		{
			Addr[0] = t & 0xFF;
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 1);
			Size++;
		}
		else if(t < 0x7FF)
		{
			//Addr just one temp parameter for saving converted utf8 data
			Addr[0] = 0xC0 | ((t >> 6) & 0x1F);
			Addr[1] = 0x80 | (t & 0x3F);
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 2);
			Size = Size + 2;
		}
		else
		{
			//Addr just one temp parameter for saving converted utf8 data
			Addr[0] = 0xE0 | ((t >> 12) & 0x0F);
			Addr[1] = 0x80 | ((t >> 6) & 0x3F);
			Addr[2] = 0x80 | (t & 0x3F);
			SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 13 + Size, Addr, 3);
			Size = Size + 3;
		}
	}

	//output size
	Addr[0] = Size; //max 256
	SpiFlashWrite(StartOffset + BTUDIN_OFFSET + 12, Addr, 1);
}

bool GetUsrDefinedBtDevName(uint8_t* Name)
{
	uint32_t t, StartOffset;
	uint8_t tmpName[6] = "\0";
	//step 1 get the legal offset
	if(!FlshGetBtPairingInfoOffsetCallBk(&StartOffset, &t))
	{
		return 0;
	}

	SpiFlashRead(StartOffset + BTUDIN_OFFSET, tmpName, 6); //Sync data
	if((tmpName[0] != 'B') || (tmpName[1] != 'T') ||
	        (tmpName[2] != 'U') || (tmpName[3] != 'D') ||
	        (tmpName[4] != 'I') || (tmpName[5] != 'F')) //first time
	{
		return 0;
	}

	//read name length
	SpiFlashRead(StartOffset + BTUDIN_OFFSET + 12, tmpName, 1);
	t = tmpName[0];
	if(t > MAX_UDIN_NAME_LEN)//cut
	{
		t = MAX_UDIN_NAME_LEN - 1;
		Name[t] = 0;
	}
	SpiFlashRead(StartOffset + BTUDIN_OFFSET + 13, tmpName, 1);
	if(tmpName[0] != 0x00)//check the name
	{
		Name[0] = tmpName[0];
		SpiFlashRead(StartOffset + BTUDIN_OFFSET + 14, Name + 1, t - 1); //read all rest data
	}

	return 1;
}

void GetUsrDefinedBtDevAddr(uint8_t* Addr)
{
}

#if 0
#define TEST_ROUND (1000)
void Test(void)
{
	uint8_t Addr[6] = "12345", Addr2[6];
	uint32_t xx;

	//SpiFlashInfoInit();

	for(xx = 0; xx < TEST_ROUND; xx++)
	{
		FlshUpDateBtPairingInfoCallBk(104, 64, 47);
		FlshLoadBtPairingInfoCallBk(104, 64);
	}
	DBG("OK1\n");

	//while(1);

	for(xx = 0; xx < TEST_ROUND; xx++)
	{
		Addr[5] = xx;
		FlshUpdateBtLatestDevAddr(Addr);
		FlshGetAliveAddr(Addr2);
		DBG("%x\n", Addr2[5]);
	}

	DBG("OK2\n");

	while(1);
}

#endif // #if 0


