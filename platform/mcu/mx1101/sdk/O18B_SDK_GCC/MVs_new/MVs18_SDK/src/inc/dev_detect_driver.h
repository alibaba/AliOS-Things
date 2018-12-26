////////////////////////////////////////////////////////////////////////////////
//                   Mountain View Silicon Tech. Inc.
//		Copyright 2013, Mountain View Silicon Tech. Inc., ShangHai, China
//                   All rights reserved.
//
//		Filename	:dev_detect_driver.h
//
//             maintainer: Halley
//
//		Description	:
//					a device detect driver(realtime to check sd/usb/linein/.. plugin or plugout event)
//
//		Changelog	:
//					2013-06-13	Created Source File
///////////////////////////////////////////////////////////////////////////////
#ifndef __DEV_DETECT_DRIVER_H__
#define __DEV_DETECT_DRIVER_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//extern bool IsMicLinkFlag;

void LockSdClk(void);
void UnLockSdClk(void);

bool IsCard2Link(void);
uint8_t IsCard2Flag(void);
void SetCard2Flag(int8_t Num);

bool IsIISInLink(void);
bool IsCardLink(void);
bool IsUDiskLink(void);

bool CheckCardLinkFlag(void);
bool CheckUSBLinkFlag(void);
// Check if any disk link(check falg var, so not realtime)
bool CheckAllDiskLinkFlag(void);

bool IsPcLink(void);
//bool IsUsbAudioLink(void);
bool IsI2sInLink(void);
bool IsLineInLink(void);
bool IsRadioLink(void);
bool IsMicInLink(void);

void SetQuickResponseFlag(bool flag);

bool GetQuickResponseFlag(void);


void DeviceDetect(void);
// 上电时，硬件扫描消抖
void InitDeviceDetect(void);

bool IsModeEnvOk(uint32_t ModeId);

#ifdef __cplusplus
}
#endif//__cplusplus

#endif
