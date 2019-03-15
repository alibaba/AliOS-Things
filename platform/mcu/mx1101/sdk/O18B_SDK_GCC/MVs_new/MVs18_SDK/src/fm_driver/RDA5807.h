/*
*****************************************************************************
*					Mountain View Silicon Tech. Inc.
*	Copyright 2009, Mountain View Silicon Tech. Inc., ShangHai, China
*					All rights reserved.
*
* Filename:			
* Description:		
* maintainer: Sam
******************************************************************************
*/
#ifndef __RDA5807_H__
#define __RDA5807_H__

#ifdef __cplusplus
extern "C" {
#endif//__cplusplus

//实测型号RDA5807HP,RDA58074M，其他型号如RDA5807NP驱动略有区别注意调整
//RDA5807P、RDA5807SP、RDA5807SS、RDA5807H系列、RDA5807N系列；
#define VER_HP	  0//ID  5801
#define VER_P	  1//ID (5802  5803) 
#define VER_PE	  2//ID  5804
#define VER_N	  3//ID  5808

#define RDA5807_CHIP_ADDR	0x22
#define RDA5807PH_REG_LEN	64
#define RDA5807PE_REG_LEN	30
#define RDA5807N_REG_LEN	32

#define MUTE_WITH_HIZ			    // Turn on only if audio outpus is required to be HiZ in mute state

bool RDA5807Write(uint8_t addr, uint16_t dat);
uint16_t RDA5807Read(uint8_t Register_Address);
bool RDA5807Init(void);
bool RDA5807ReadID(void);
void RDA5807Mute(bool mute);
bool RDA5807PowerOn(void);
bool RDA5807PowerDown(void);
void RDA5807SetFreq(uint16_t freq);
void RDA5807SearchSet(uint16_t freq);
uint8_t RDA5807SearchRead(uint16_t freg);
void RDA5807VolSet(uint8_t vol);
void RDA5807OutputHIZ(bool HIZOnOff);

#ifdef  __cplusplus
}
#endif//__cplusplus

#endif

