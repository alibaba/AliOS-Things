/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_wdt.h
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  WDT模块库函数头文件
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#ifndef __LIBWDT_H__
#define __LIBWDT_H__

#include "HR8P506.h"
#include "type.h"

/* WDT时钟选择 */
typedef enum {
	WDT_CLOCK_PCLK = 0x0,	//PCLK
	WDT_CLOCK_WDT  = 0x1,	//WDT时钟源，32kHz
} WDT_TYPE_CLKS;

/* 初始化结构体 */
typedef struct {
	uint32_t WDT_Tms;		//定时时间，单位ms 
	TYPE_FUNCEN WDT_IE;		//中断使能
	TYPE_FUNCEN WDT_Rst;	//复位使能
	WDT_TYPE_CLKS WDT_Clock;//时钟选择
} WDT_InitStruType;


#define WDT_RegUnLock()		(WDT->LOCK.Word = 0x1ACCE551)
#define WDT_RegLock()		(WDT->LOCK.Word = 0x0)

#define WDT_Enable()		(WDT->CON.EN = 1)
#define WDT_Disable()		(WDT->CON.EN = 0)

#define WDT_Clear()			{WDT_RegUnLock();WDT->INTCLR.Word = 0;WDT_RegLock();}

#define WDT_ITEnable()		(WDT->CON.IE = 1)
#define WDT_ITDisable()		(WDT->CON.IE = 0)

#define WDT_RstEnable()		(WDT->CON.RSTEN = 1)
#define WDT_RstDisable()	(WDT->CON.RSTEN = 0)

#define WDT_CLOCK_PCLK()	(WDT->CON.CLKS = 0)
#define WDT_CLOCK_WDT()		(WDT->CON.CLKS = 1)


void WDT_Init(WDT_InitStruType *WDT_InitStruct);
void WDT_SetReloadValue(uint32_t Value);
uint32_t WDT_GetValue(void);
FlagStatus WDT_GetFlagStatus(void);

#endif
