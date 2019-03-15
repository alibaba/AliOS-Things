///////////////////////////////////////////////////////////////////////////////
//               Mountain View Silicon Tech. Inc.
//  Copyright 2012, Mountain View Silicon Tech. Inc., Shanghai, China
//                       All rights reserved.
//  Filename: radio_driver_simulator.h
//  maintainer: Sam
///////////////////////////////////////////////////////////////////////////////

#ifndef __RADIO_DRV_H__
#define __RADIO_DRV_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

#include "app_config.h"
#include "radio_control.h"


// 搜索结果返回值
typedef enum _RADIO_SEEK_RESULT
{
    RADIO_SEEK_NONE = 0,
    RADIO_SEEK_FALSE,
    RADIO_SEEK_STC,
    RADIO_SEEK_VALIDSTATION,
    RADIO_SEEK_BANDLIMIT
} RADIO_SEEK_RESULT;


//// 驱动接口，屏蔽FM设备型号差异，不同的设备驱动，在内部区分////
// 初始化FM设备
bool RadioInit(void);
bool RadioValidCheck(void);
// 打开FM设备
bool RadioPowerOn(void);
// 关闭FM设备
bool RadioPowerOff(void);
// 检查FM设备状态(TRUE - 工作状态、FALSE - 挂起状态)
bool IsRadioWorking(void);
// 设置当前频率
bool RadioSetFreq(uint16_t Freq);
// 指定频率搜索
void RadioSearchSet(uint16_t Freq);
// 静音切换
bool RadioMute(bool MuteFlag);
// 判断信号强度，用于搜台结果查询，如果返回TRUE，表明搜到新的电台，否则未搜到
bool RadioRSSIRead(uint16_t Freq);
// FM通道输出高阻
void RadioLinkOutputHIZ(bool HizFg);
// 是否立体声
bool RadioIsStereo(void);
//	设置立体声
void RadioSetStereo(bool StereoFlag);
////////////////////////////////////////////////////////////////

#ifdef __cplusplus
}
#endif//__cplusplus

#endif/*__RADIO_DRV_SIMULATOR_H_*/





