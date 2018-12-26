///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: radio_control.h
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////
#ifndef __RADIO_CONTROL_H__
#define __RADIO_CONTROL_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "../fm_driver/radio.h"

#ifdef FUNC_RADIO_EN

#define FM_AREA_CHINA  0x00 // 00B—87.5~108MHz (US/Europe, China)     
#define FM_AREA_JAPAN  0x01 // 01B—76~90MHz (Japan)
#define FM_AREA_RUSSIA 0x02 // 10B—65.8~73MHz (Russia)                
#define FM_AREA_OTHER  0x03 // 11B—60~76MHz

#define CHINA_BASE_FREQ  875 // China/US/Europe频段 基准频率
#define JAPAN_BASE_FREQ  760 // Japan频段 基准频率
#define RUSSIA_BASE_FREQ 650 // Russia频段 基准频率
#define OTHER_BASE_FREQ  600 // 其它频段 基准频率
	
	
#define FM_CHANNLE_STEP  100 // 100KHZ，FM搜台步进

#pragma pack(1)
// 支持以下FM设备
typedef enum _Radio_NAME
{
    RADIO_NONE,
    RADIO_RDA5807P,
    RADIO_SILI4702,
    RADIO_CS2010,
    RADIO_CL6017X,
    RADIO_KT0830E,
    RADIO_AR1000F,
    RADIO_BK1080,
    RADIO_QN8035,
    RADIO_RDA5807H
} RADIO_NAME;

typedef enum _RADIO_STATUS_
{
    RADIO_STATUS_IDLE = 0,  // 空闲中
    RADIO_STATUS_SEEKING,   // 搜台中
    RADIO_STATUS_PLAYING,   // 播放中
    RADIO_STATUS_PREVIEW,   // 电台预览

} RADIO_STATUS;
#pragma pack()


#define RADIO_DEF_MIN_FREQ_VALUE 875  // FM频率最小范围(US/Europe, China)
#define RADIO_DEF_MAX_FREQ_VALUE 1080 // FM频率最大范围(US/Europe, China)

#define RADIO_JAP_MIN_FREQ_VALUE 760  // FM频率最小范围(Japan)
#define RADIO_JAP_MAX_FREQ_VALUE 900  // FM频率最大范围(Japan)

#define RADIO_RUS_MIN_FREQ_VALUE 658  // FM频率最小范围(Russia)
#define RADIO_RUS_MAX_FREQ_VALUE 730  // FM频率最大范围(Russia)

#define RADIO_OTH_MIN_FREQ_VALUE 600  // FM频率最小范围(其它)
#define RADIO_OTH_MAX_FREQ_VALUE 760  // FM频率最大范围(其它)

#pragma pack(1)
typedef struct _RADIO_CONTROL_
{
	uint8_t   Channel[MAX_RADIO_CHANNEL_NUM]; // 电台列表 ??? 是否可以改为指针，直接指向 NVM中的数据，节省内存
	uint16_t  Freq;                           // 当前频率
	uint16_t  MinFreq;
	uint16_t  MaxFreq;
	uint8_t  ChlCount;                       // 已有电台个数
	uint8_t  CurStaIdx;                      // 当前电台索引号
	uint8_t  State;                          // FM Radio当前状态
	uint8_t  CurFreqArea;                    // 当前频段(中国、日本、欧洲...)

	SW_TIMER TimerHandle;
#ifdef RADIO_DELAY_SWITCH_CHANNEL
	SW_TIMER DelayDoTimer; // 电台切换延迟跳转Timer
#endif

	uint16_t InputNumber; // 红外数字按键选台
	SW_TIMER NumKeyTimer;

} RADIO_CONTROL;
#pragma pack()

extern RADIO_CONTROL* sRadioControl;
extern RADIO_NAME Radio_Name;	//区分FM的型号

bool RadioDeinitialize(void);
bool RadioInitialize(void);
bool RadioControl(void);
void RadioPlay(void);
bool RadioSwitchChannel(uint32_t Msg);

#endif/*FUNC_RADIO_EN*/

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
