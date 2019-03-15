/**
 *******************************************************************************
 * @file    sys_app.h
 * @author  Richard
 * @version V1.0.0
 * @date    17-12-2013
 * @brief   system application function declaration
 * @maintainer: Sam
 *******************************************************************************
 * @attention
 *
 * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
 * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
 * TIME. AS A RESULT, MVSILICON SHALL NOT BE HELD LIABLE FOR ANY DIRECT,
 * INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
 * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
 * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 *
 * <h2><center>&copy; COPYRIGHT 2013 MVSilicon </center></h2>
 */

#ifndef __SYS_APP_H__
#define __SYS_APP_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "spi_flash.h"

//语言类型
typedef enum _LANG
{
    LANG_ZH, // 中文标识
    LANG_EN, // 英文标识
	
    LANG_SUM,
} LANG;


//菜单类型，选择音量加、音量减按键的功能
typedef enum _AUDIO_SET_MODE
{
	AUDIO_SET_MODE_MAIN_VOL,	//主音量调节
	AUDIO_SET_MODE_MIC_VOL,		//MIC音量调节
	AUDIO_SET_MODE_ECHO_DEPTH,	//MIC混响调节
	AUDIO_SET_MODE_ECHO_DELAY,	//MIC延时调节
	AUDIO_SET_MODE_TREB,		//高音调节
	AUDIO_SET_MODE_BASS,		//低音调节
	
	AUDIO_SET_MODE_SUM			//模式数量
} AUDIO_SET_MODE;

typedef enum _MODE_SWITCH_STATE{
	MODE_SWITCH_STATE_START = 0,
	MODE_SWITCH_STATE_DONE
} MODE_SWITCH_STATE;

//#pragma pack(1)
typedef struct _SYS_INFO_
{
	uint8_t CurModuleID;	//current running module id
	uint8_t ModuleIDBackUp;
	uint8_t NextModuleID;	//next module id

	// 当前工作磁盘类型(播放模式、录音等文件相关的都会用到)
	uint8_t DiskType;		// USB_TYPE - U盘，SD_TYPE - SD卡

    bool MicEnable;
	uint8_t MicVolume;
	
	uint8_t Volume;
	uint8_t MuteFlag;
	uint8_t Eq;
#ifdef FUNC_TREB_BASS_EN
	uint8_t TrebVal;
	uint8_t BassVal;
	uint8_t EqStatus;		// set effect to eq or treb/bass. 0: treb/bass, 1:eq
#endif
	
	uint8_t FmSysClkDivFactor;	//system clock divide frequency factor in FM mode
	uint8_t IsSysClk96MHz;
	uint8_t AudioSetMode;
    uint8_t RecordState;
    uint8_t RecordSource;
	bool SoundRemindOn;
    uint8_t LanguageMode;

#ifdef FUNC_BT_HF_EN
	uint8_t HfVolume;
#endif
#ifdef FUNC_AUDIO_DRC_EFFECT_EN
    uint8_t AudioDRCEffectFlag;
    DRCContext AudioDRCEffectHandle;
#endif
#ifdef FUNC_AUDIO_3D_EFFECT_EN
    uint8_t Audio3dEffectFlag;
    ThreeDContext Audio3dEffectHandle;
#endif

#ifdef FUNC_AUDIO_VB_EFFECT_EN
    uint8_t AudioVbEffectFlag;
    VBContext AudioVbEffectHandle;

#endif

} SYS_INFO;

extern SYS_INFO gSys;

void SysVarInit(void);		//全局变量的初始化，开机时调用
void SysVarDeinit(void);	//全局变量的保存，关机时调用

void SystemOn(void);		//系统开机判断，如果不满足开机条件，则本函数中继续进入睡眠模式
void AudioSysInfoSetBreakPoint(void);
//void AudioLoad InfoFromFlash(void);
void AlarmRemindOnOff(void);

//Flash 解锁
//参数: 无
//返回值: 解锁成功返回TRUE，否则返回FALSE
bool FlashUnlock(void);

//Flash加锁
//参数:lock_range ，Flash加锁范围: 
//		 FLASH_LOCK_RANGE_HALF : 			加锁1/2 Flash 空间(从0开始，下同)
//		FLASH_LOCK_RANGE_THREE_QUARTERS: 	加锁3/4 Flash 空间
//		FLASH_LOCK_RANGE_SEVENTH_EIGHT:	加锁7/8 Flash 空间
//		FLASH_LOCK_RANGE_ALL:				加锁全部Flash 空间
//注意用户需要根据USER BANK的地址来决定加锁的范围，否则可能将USER bank空间加锁而无法写数据
//返回值: 加锁成功返回TRUE，否则返回FALSE
bool FlashLock(SPI_FLASH_LOCK_RANGE lock_range);


//从消息队列中接收消息
//有消息立即返回消息值
//无消息等待WaitTime毫秒
uint16_t MsgRecv(uint16_t WaitTime);

//向消息队列中发送指定消息
void MsgSend(uint16_t Msg);

// 检查消息队列是否有指定消息
bool MsgCheck(uint16_t Msg);

//从消息队列中删除指定消息
void MsgClear(uint16_t Msg);

//从消息队列中删除指定消息集的所有消息
void MsgClearClass(uint16_t MsgClass);

//////////// mode switch state /////////
void SetModeSwitchState(MODE_SWITCH_STATE state);
MODE_SWITCH_STATE GetModeSwitchState(void);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

