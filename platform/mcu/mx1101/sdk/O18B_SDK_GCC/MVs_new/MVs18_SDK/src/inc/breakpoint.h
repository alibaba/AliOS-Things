///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: breakpoint.h
//  ChangLog :
//			修改bp 模块操作方式2014-9-26 ，lujiangang
///////////////////////////////////////////////////////////////////////////////
#ifndef __BREAKPOINT_H__
#define __BREAKPOINT_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

// Disk类型
#define UNKNOWN_DISK_TYPE -1
#define NONE_DISK 0 // 没有插入磁盘
#define USB_TYPE  1 // U盘为播放盘
#define SD_TYPE   2 // SD卡为播放盘

#ifdef FUNC_BREAKPOINT_EN

#define BP_MAX_SIZE		115		//BP 信息最大字节数(考虑到NVM空间有限)
#define BP_SIGNATURE	0x5A
#define BP_SET_ELEMENT(a, b) a = b
#define BP_GET_ELEMENT(x) x
#pragma pack(1)


//**************************************************************************************************************
//	PLAYER 相关断点记忆数据结构
//	注意:: 修改BP_PLAYER_INFO 后请更新sInitPlayerInfo 常量
//**************************************************************************************************************
#if defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)	//PLAYER 数据存储结构

#define PLAYER_DISK_BP_CNT 3

typedef struct _BP_PLAY_DISK_INFO_
{
	uint32_t FileAddr; 		// 文件扇区号
	uint16_t PlayTime; 		// 播放时间
	uint8_t  CRC8;     		// 文件名校验码
	uint8_t  FolderEnFlag; 	//文件夹使能标志
} BP_PLAY_DISK_INFO;

typedef struct _BP_PLAYER_INFO_ // 播放模式 数据存储结构
{
	// 播放音量
	uint8_t	PlayerVolume; // Volume:0--32
	// 当前播放模式
	uint8_t	PlayMode : 7; // Play mode
	// 歌词开关
	uint8_t 	LrcFlag  : 1; // Lrc
	// 播放盘信息
	BP_PLAY_DISK_INFO PlayDiskInfo[PLAYER_DISK_BP_CNT];
} BP_PLAYER_INFO;
#endif




//**************************************************************************************************************
//	Radio 相关断点记忆数据结构
//	注意:: 修改BP_RADIO_INFO 后请更新sInitRadioInfo 常量
//**************************************************************************************************************
#ifdef FUNC_RADIO_EN
typedef struct _BP_RADIO_INFO_
{
	uint8_t		StationList[50]; 		// 已保存电台列表/*MAX_RADIO_CHANNEL_NUM*/
	uint8_t		RadioVolume   : 6;  	// FM音量
	uint8_t		CurBandIdx : 2;  		// 当前FM波段范围(00B—87.5~108MHz (US/Europe, China)、01B—76~90MHz (Japan)、10B—65.8~73MHz (Russia)、11B—60~76MHz
	uint8_t		StationCount;    		// 已保存电台个数
	uint16_t	CurFreq;         				// 当前电台频率
} BP_RADIO_INFO;
#endif




//**************************************************************************************************************
//	SYSTEM 相关断点记忆数据结构
//	注意:: 修改BP_SYS_INFO 后请更新sInitSysInfo 常量
//**************************************************************************************************************
typedef struct _BP_SYS_INFO_
{
	uint8_t 	VaildFlag;   // Only when this field is equal to BP_SIGNATURE, this structure is valid.
	// 当前应用模式
	uint8_t 	CurModuleId; // system function mode.
	// 当前工作磁盘类型(播放模式、录音等文件相关的都会用到)
	uint8_t 	DiskType;    // USB_TYPE - U盘，SD_TYPE - SD卡
	uint8_t  	Volume;
	uint8_t	    Eq;          // EQ
    
#ifdef FUNC_SOUND_REMIND
	uint8_t 	SoundRemindOn;
	uint8_t 	LanguageMode;
#endif
#ifdef FUNC_BT_HF_EN
	uint8_t 	HfVolume;
#endif
} BP_SYS_INFO;






//**************************************************************************************************************
//	Breakpoint 断点记忆数据结构
//**************************************************************************************************************
typedef struct	_BP_INFO_
{
	BP_SYS_INFO    	SysInfo;
	
#if (defined(FUNC_USB_EN) || defined(FUNC_CARD_EN)) 	// 播放模式数据存储区域
	BP_PLAYER_INFO PlayerInfo;
#endif

#ifdef FUNC_RADIO_EN // FM模式数据存储区域
	BP_RADIO_INFO 	RadioInfo;
#endif

	uint8_t 			Crc;
	
} BP_INFO;



typedef enum _BP_SAVE_TYPE_
{
	BP_SAVE2NVM,
	BP_SAVE2FLASH,
	BP_SAVE2EEPROM,
	BP_SAVE2NVM_FLASH,
	BP_SAVE2NVM_EEPROM
}BP_SAVE_TYPE;

typedef enum _BP_INFO_TYPE_
{
	BP_SYS_INFO_TYPE,
	BP_PLAYER_INFO_TYPE,
	BP_RADIO_INFO_TYPE,
}BP_INFO_TYPE;

#pragma pack()

bool EraseFlashOfUserArea(void);
void BP_InfoLog(void);
uint8_t GetCrc8CheckSum(uint8_t* ptr, uint32_t len);	// 计算CRC
void BP_LoadInfo(void);
void* BP_GetInfo(BP_INFO_TYPE InfoType);
bool BP_SaveInfo(BP_SAVE_TYPE SaveType);

#endif
#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__BREAKPOINT_H_*/

