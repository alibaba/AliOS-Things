///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: breakpoint.c
//  ChangLog :
//			修改bp 模块操作方式2014-9-26 ，lujiangang
///////////////////////////////////////////////////////////////////////////////

#include "app_config.h"
#include "breakpoint.h"
#include "fat_file.h"
#include "audio_decoder.h"
#include "player_control.h"
#include "nvm.h"
#include "eq_params.h"

#ifdef FUNC_BREAKPOINT_EN

#ifdef BP_SAVE_TO_FLASH // 掉电记忆
static bool LoadBPInfoFromFlash(void);
static bool SaveBPInfoToFlash(void);
#endif
static BP_INFO bp_info;

// global sysInfo default init nvm value
const static BP_SYS_INFO sInitSysInfo =
{
	0x5A,                           			// ValidFlag

#ifdef FUNC_BT_EN
	(uint8_t)MODULE_ID_BLUETOOTH,               // CurModuleId
#else
	(uint8_t)MODULE_ID_PLAYER_USB,              // CurModuleId
#endif
	SD_TYPE,                        			// DiskType
       
	DEFAULT_VOLUME,     				        // Sys Volume default value
    EQ_STYLE_OFF,                               // Eq
#ifdef FUNC_SOUND_REMIND
	1,                             				// 1:Enable Alarm Remind;
	1,                            				// 1:Chinese Mode
#endif
#ifdef FUNC_BT_HF_EN
	15,								//bt hf volume
#endif
};

#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)) // player module default init nvm value

const static BP_PLAYER_INFO sInitPlayerInfo =
{
	25, 						// Volume
	PLAY_MODE_REPEAT_ALL,  	// PlayMode
	0, 						// LrcFlag
	{{0}, {0}, {0}}, 			// PlayDisk[0:2]
};

#endif

#ifdef FUNC_RADIO_EN // radio module default init nvm value
const static BP_RADIO_INFO sInitRadioInfo =
{
	{0}, 					// 已保存电台列表/*MAX_RADIO_CHANNEL_NUM*/
	10,  						// FM音量
	0,   						// 当前FM波段范围(00B—87.5~108MHz (US/Europe, China)、01B—76~90MHz (Japan)、10B—65.8~73MHz (Russia)、11B—60~76MHz
	0,  						// 已保存电台个数
	0,   						// 当前电台频率
};
#endif


// 计算CRC
uint8_t GetCrc8CheckSum(uint8_t* ptr, uint32_t len)
{
	uint32_t crc = 0;
	uint32_t i;

	while(len--)
	{
		crc ^= *ptr++;
		for(i = 0; i < 8; i++)
		{
			if(crc & 0x01)
			{
				crc = ((crc >> 1) ^ 0x8C);
			}
			else
			{
				crc >>= 1;
			}
		}
	}
	return (uint8_t)crc;
}


#ifdef FUNC_DEBUG_EN
void BP_InfoLog(void)
{
	APP_DBG("**********************************\n");
	APP_DBG("*  Signature:%X\n", bp_info.SysInfo.VaildFlag);
	APP_DBG("*  Cur Mode:%X\n", bp_info.SysInfo.CurModuleId);
	APP_DBG("*  DiskType:%X\n", bp_info.SysInfo.DiskType);
	APP_DBG("*\n");
	APP_DBG("**********************************\n");
}
#endif


// 系统启动时装载, 从NVM 或Flash加载BP 信息
//如果加载失败则加载默认BP 值
//注意，该函数可能会读取Flash，因此需要在SpiFlashInfoInit()函数之后调用
void BP_LoadInfo(void)
{
	bool ret = TRUE;

	APP_DBG("BP_LoadInfo\n");

	ASSERT(sizeof(bp_info) <= BP_MAX_SIZE);

//	if(bp_info.SysInfo.VaildFlag == BP_SIGNATURE)
//	{
//		return;
//	}

	NvmRead(0, (uint8_t*)&bp_info, sizeof(bp_info));
	if(bp_info.Crc != GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(bp_info)))
	{
#ifdef BP_SAVE_TO_FLASH
		ret = LoadBPInfoFromFlash();
#else
		ret = FALSE;
#endif
	}

	//获取BP 信息失败，则赋默认值
	if(!ret)
	{
		memcpy((void*)&bp_info.SysInfo, (void*)&sInitSysInfo, sizeof(BP_SYS_INFO));
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))
		memcpy((void*)&bp_info.PlayerInfo, (void*)&sInitPlayerInfo, sizeof(BP_PLAYER_INFO));
#endif

#ifdef FUNC_RADIO_EN
		memcpy((void*)&bp_info.RadioInfo, (void*)&sInitRadioInfo, sizeof(BP_RADIO_INFO));
#endif
		bp_info.SysInfo.VaildFlag = BP_SIGNATURE;
	}

	return;
}


//返回bp_info 中相关模块BP 信息的指针
//当需要更新BP 信息时，直接通过该指针往bp_info 变量中写
void* BP_GetInfo(BP_INFO_TYPE InfoType)
{
	void *pInfo = NULL;
	switch(InfoType)
	{
		case BP_SYS_INFO_TYPE:
			pInfo = (void *)&(bp_info.SysInfo);
			break;
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN))			
		case BP_PLAYER_INFO_TYPE:
			pInfo = (void *)&(bp_info.PlayerInfo);
			break;
#endif			
#ifdef FUNC_RADIO_EN			
		case BP_RADIO_INFO_TYPE:
			pInfo = (void *)&(bp_info.RadioInfo);
			break;
#endif			
		default:
			break;
	}
	return pInfo;
}



//备份BP 信息到NVM ，FLASH 或EEPROM
//成功返回TRUE， 失败返回FALSE
bool BP_SaveInfo(BP_SAVE_TYPE SaveType)
{
	bool ret;
	uint8_t crc;

	ASSERT(sizeof(bp_info) <= BP_MAX_SIZE);
	
	//在Save到NVM、FLASH或EEPROM之前必须先计算出CRC
	crc = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO)-1);
	bp_info.Crc = crc;
	switch(SaveType)
	{
		case BP_SAVE2NVM:
			ret = NvmWrite(0, (uint8_t *)&bp_info, sizeof(BP_INFO));
			break;
		case BP_SAVE2FLASH:
			#ifdef BP_SAVE_TO_FLASH // 掉电记忆
			ret = SaveBPInfoToFlash();
			#endif
			break;
		case BP_SAVE2EEPROM:
			//empty now
			break;
		case BP_SAVE2NVM_FLASH:
			ret = NvmWrite(0, (uint8_t *)&bp_info, sizeof(BP_INFO));
			if(ret)
			{
			#ifdef BP_SAVE_TO_FLASH // 掉电记忆
				ret = SaveBPInfoToFlash();
			#endif
			}
			break;
		case BP_SAVE2NVM_EEPROM:
			//empty now
			break;
		default:
			break;
	}
	return ret;
}




#ifdef BP_SAVE_TO_FLASH // 掉电记忆

#include "spi_flash.h"

#ifndef DWNLDR_USRDAT_ADDR
#define DWNLDR_USRDAT_ADDR      (0x90)
#endif

//（镜像格式：NVM Flag(4Bytes "MV18") + NVM BAK ID(4Bytes) + NVM Size(2Bytes) + NVM SUB BAK ID(1Bytes) + CRC(1Bytes) + NVM Data(Sub1/Sub2/...)）
//（镜像包头：
typedef struct __BP_BAK_WRAPPER_
{
	char 	Magic[4];
	uint32_t 	BPBakId;
	uint16_t 	BPBakSize;
	uint8_t 	BPBakSubId;
	uint8_t 	Crc;
}BP_BAK_WRAPPER;

#define NVM_BAK_OFFSET		(64*1024)	// BP 信息在Flash中的起始地址
#define NVM_BAK_CNT		(4)			// BP 信息存储区域个数
#define NVM_BLOCK_SIZE		(4096)		//BP 信息存数区域大小

// BP 记录信息Size : bp_info 记录大小+ 记录包头
#define NVM_BAK_SIZE     		192	//(sizeof(BP_INFO) + sizeof(BP_BAK_WRAPPER))	
#define NVM_SUB_BAK_CNT  	((NVM_BLOCK_SIZE)/NVM_BAK_SIZE)


static uint32_t NVM_BAK_ADDR0   = NVM_BAK_OFFSET;	//BP INFO 在Flash中的起始地址

// OutputBakId: 输出NVM BAK ID
// OutputSubBakId: 输出NVM SUB BAK ID
// return: NVM镜像地址,LJG 返回最新NVM镜像所在block的首地址，一共4个block
static uint32_t FindLatestBPBak(uint32_t* OutputBakId, uint8_t* OutputSubBakId, bool IsLoadFlash)
{
	uint32_t i, j;
	int32_t TempIndex = -1;
	uint32_t TempBakId;
	BP_BAK_WRAPPER BpBakWrapper;
	uint8_t  TempSubBakId;

	*OutputBakId = 0;
	if(!IsLoadFlash)
	{
		*OutputSubBakId = (NVM_SUB_BAK_CNT - 1);
	}
	else
	{
		*OutputSubBakId = 0;
	}

	for(i = 0; i < NVM_BAK_CNT; i++)
	{
		// read flash to get nvm bak id
		if(SpiFlashRead(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0 ||
		        memcmp(BpBakWrapper.Magic, "MV18", 4) != 0)
		{
			continue;
		}

		TempBakId    = BpBakWrapper.BPBakId;
		TempSubBakId = BpBakWrapper.BPBakSubId;

		for(j = 0; j < (NVM_BLOCK_SIZE / NVM_BAK_SIZE); j++)
		{
			if(SpiFlashRead(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE + j * NVM_BAK_SIZE, (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0 ||
			        memcmp(BpBakWrapper.Magic, "MV18", 4) != 0)
			{
				break;
			}

			TempBakId    = BpBakWrapper.BPBakId;;
			TempSubBakId = BpBakWrapper.BPBakSubId;
		}

		if(*OutputBakId > TempBakId)
		{
			continue;
		}

		TempIndex       = i;
		*OutputBakId    = TempBakId;
		*OutputSubBakId = TempSubBakId;
	}

	if(TempIndex == -1)
	{
		TempIndex = (NVM_BAK_CNT - 1);
	}
	else if(!IsLoadFlash)
	{
		*OutputSubBakId = (NVM_SUB_BAK_CNT - 1);

		i = TempIndex;
		if(i == 0)
		{
			i = NVM_BAK_CNT - 1;
		}
		else
		{
			i--;
		}
		for(j = 0; j < (NVM_BLOCK_SIZE / NVM_BAK_SIZE); j++)
		{
			if(SpiFlashRead(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE + j * NVM_BAK_SIZE, (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0)
			{
				return (NVM_BAK_ADDR0 + TempIndex * NVM_BLOCK_SIZE);
			}

			if(memcmp(BpBakWrapper.Magic, "MV18", 4) != 0)
			{
				break;
			}

			*OutputSubBakId = BpBakWrapper.BPBakSubId;
		}
	}
	return (NVM_BAK_ADDR0 + TempIndex * NVM_BLOCK_SIZE);
}

static bool LoadBakBPInfo(uint32_t BakNvmAddr, uint8_t SubBakId)
{
	uint8_t CrcVal;
	BP_BAK_WRAPPER BpBakWrapper;
	
	if(SpiFlashRead((uint32_t)((uint8_t*)BakNvmAddr + SubBakId * NVM_BAK_SIZE), (uint8_t*)&BpBakWrapper, sizeof(BpBakWrapper)) < 0)
	{
		return FALSE;
	}
	if(SpiFlashRead((uint32_t)((uint8_t*)BakNvmAddr + SubBakId * NVM_BAK_SIZE + sizeof(BpBakWrapper)), (uint8_t*)&bp_info, sizeof(bp_info)) < 0)
	{
		return FALSE;
	}
	
	CrcVal = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO));
	if(CrcVal != BpBakWrapper.Crc)
	{
		return FALSE;
	}
	
	if(!NvmWrite(0, (uint8_t*)&bp_info, sizeof(bp_info)))
	{
		return FALSE;
	}

	return TRUE;
}

static bool SaveBakBPInfo(uint32_t BakNvmAddr, uint32_t NvmBakId, uint8_t SubBakId)	//SaveBakNVM
{
	uint8_t CrcVal;
	uint16_t NvmSize = sizeof(BP_INFO);
	uint8_t Temp[sizeof(BP_BAK_WRAPPER)+sizeof(BP_INFO)];
	BP_BAK_WRAPPER *pBakWrapper;

	CrcVal = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO)-1);
	bp_info.Crc = CrcVal;	//首先设置bp_info内部CRC

	//计算整个bp_info的crc
	CrcVal = GetCrc8CheckSum((uint8_t*)&bp_info, sizeof(BP_INFO));
	pBakWrapper = (BP_BAK_WRAPPER*)&Temp[0];
	memcpy((void*)&pBakWrapper->Magic, (void*)"MV18", 4);
	pBakWrapper->BPBakId = NvmBakId;
	pBakWrapper->BPBakSize = NvmSize;
	pBakWrapper->BPBakSubId = SubBakId;
	pBakWrapper->Crc = CrcVal;
	memcpy((void*)&Temp[sizeof(BP_BAK_WRAPPER)],(void*)&bp_info, sizeof(BP_INFO));

	if(SubBakId == 0)
	{
		uint8_t buf[4];
		SpiFlashRead(BakNvmAddr,buf,4);
		if(buf[0] !=0xFF ||buf[1] !=0xFF ||buf[2] !=0xFF ||buf[3] !=0xFF)
		{
			SpiFlashErase(BakNvmAddr, NVM_BLOCK_SIZE);
		}
		if(SpiFlashWrite(BakNvmAddr, Temp, sizeof(Temp)) < 0)
		{
			return FALSE;
		}
		return TRUE;
	}

	if(SpiFlashWrite((uint32_t)((uint8_t*)BakNvmAddr + SubBakId * NVM_BAK_SIZE), Temp, sizeof(Temp)) < 0)
	{
		return FALSE;
	}
	return TRUE;
}

static bool LoadBPInfoFromFlash(void)	
{
	uint32_t i;
	uint32_t LoadNvmBakId = 0;
	uint32_t NvmBakAddr;
	uint32_t BtDataAddr;
	uint8_t  NvmSubBakId;

	NVM_BAK_ADDR0 = NVM_BAK_OFFSET;

	// 先获取蓝牙的地址，在它之后就是NVM镜像区，以免冲突
	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&BtDataAddr, 4) < 0)
	{
		return FALSE;//error
	}

	NVM_BAK_ADDR0 += BtDataAddr;

	// 查找最近的NVM镜像
	NvmBakAddr = FindLatestBPBak(&LoadNvmBakId, &NvmSubBakId, TRUE);

	if(LoadNvmBakId == 0) // 从来没有保存过，直接返回
	{
		uint8_t tmp[4];

		if(SpiFlashRead(NVM_BAK_ADDR0 +  (NVM_BAK_CNT -1) *NVM_BLOCK_SIZE, tmp, 4) < 0)
		{
			return FALSE;//error
		}
		if(tmp[0] != 0xFF ||tmp[1] != 0xFF || tmp[0] != 0xFF || tmp[0] != 0xFF )
		{
			SpiFlashErase(NVM_BAK_ADDR0, NVM_BAK_CNT * NVM_BLOCK_SIZE);
		}
		
		return FALSE;
	}

	// 导出最近的NVM镜像
	if(LoadBakBPInfo(NvmBakAddr, NvmSubBakId))
	{
		return TRUE;
	}
	else
	{
		// 如果导出镜像失败，则尝试按先后顺序依次导出其它镜像，直到导出成功或遍历了所有的镜像都失败
		for(i = (NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE; i < NVM_BAK_CNT; i++)
		{
			if(LoadBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, (NVM_SUB_BAK_CNT - 1)))
			{
				return TRUE;
			}
		}

		for(i = 0; i < (NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE; i++)
		{
			if(LoadBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, (NVM_SUB_BAK_CNT - 1)))
			{
				return TRUE;
			}
		}
	}

	APP_DBG("Load BP From Flash Fail\n");
	return FALSE;
}

static bool SaveBPInfoToFlash(void)
{
	int32_t i;
	uint32_t SaveNvmBakId = 0;
	uint32_t NvmBakAddr;
	uint32_t BtDataAddr;
	uint8_t  SaveNvmSubBakId = NVM_SUB_BAK_CNT - 1;

	NVM_BAK_ADDR0 = NVM_BAK_OFFSET;

	// 先获取蓝牙的地址，在它之后就是NVM镜像区，以免冲突
	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&BtDataAddr, 4) < 0)
	{
		return FALSE;//error
	}

	NVM_BAK_ADDR0 += BtDataAddr;

	// 查找最近的NVM镜像
	NvmBakAddr = FindLatestBPBak(&SaveNvmBakId, &SaveNvmSubBakId, FALSE);

	if(SaveNvmBakId != 0)
	{
		if((NvmBakAddr - NVM_BAK_ADDR0) == 0)
		{
			NvmBakAddr = NVM_BAK_ADDR0 + (NVM_BAK_CNT - 1) * NVM_BLOCK_SIZE;
		}
		else
		{
			NvmBakAddr -= NVM_BLOCK_SIZE;
		}
	}

	SaveNvmBakId++;

	SaveNvmSubBakId++;

	if(SaveNvmSubBakId == NVM_SUB_BAK_CNT)
	{
		SaveNvmSubBakId = 0;
	}

	// 导入NVM镜像
	if(SaveBakBPInfo(NvmBakAddr, SaveNvmBakId, SaveNvmSubBakId))
	{
		return TRUE;
	}
	else
	{
		// 如果导入镜像失败，则尝试按先后顺序依次导入到其它镜像区域，直到导入成功或遍历了所有的镜像区域都失败
		for(i = ((NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE - 1); i >= 0; i--)
		{
			if(SaveBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, SaveNvmBakId, 0))
			{
				return TRUE;
			}
		}

		for(i = (NVM_BAK_CNT - 1); i >= (NvmBakAddr - NVM_BAK_ADDR0) / NVM_BLOCK_SIZE; i--)
		{
			if(SaveBakBPInfo(NVM_BAK_ADDR0 + i * NVM_BLOCK_SIZE, SaveNvmBakId, 0))
			{
				return TRUE;
			}
		}
	}

	APP_DBG("Save BP To Flash Fail\n");
	return FALSE;
}

bool EraseFlashOfUserArea(void)
{
#define DWNLDR_USRDAT_ADDR      (0x90)

	uint32_t BtDataAddr;

	NVM_BAK_ADDR0 = 64 * 1024;

	if(SpiFlashRead(DWNLDR_USRDAT_ADDR, (uint8_t*)&BtDataAddr, 4) < 0)
	{
		return FALSE;//error
	}
	NVM_BAK_ADDR0 += BtDataAddr;
	SpiFlashErase(NVM_BAK_ADDR0, 16 * 1024);
	return TRUE;
}
#endif

#endif/*FUNC_BREAKPOINT_EN*/










