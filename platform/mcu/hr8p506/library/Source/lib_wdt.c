/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 *文件名:  lib_wdt.c
 *作  者:  ESM Application Team 
 *版  本:  V1.01
 *日  期:  2016/11/01
 *描  述:  WDT模块库函数
 *备  注:  适用于HRSDK-GDB-8P506
 本软件仅供学习和演示使用，对用户直接引用代码所带来的风险或后果不承担任何法律责任。
 **********************************************************/
#include "lib_wdt.h"
#include "system_hr8p506.h"

/*************************************
  函数名：WDT_Init
  描  述：UART初始化
  输入值：初始化结构体
  返回值：无
 **************************************/
void WDT_Init(WDT_InitStruType *WDT_InitStruct)
{
	uint32_t tmp;

	if (WDT_InitStruct->WDT_Clock != WDT_CLOCK_PCLK)
		tmp = (uint32_t)(32 * WDT_InitStruct->WDT_Tms);
	else
		tmp = SystemCoreClock / 1000 * WDT_InitStruct->WDT_Tms;

	WDT->LOAD.Word = tmp;
	WDT->CON.CLKS = WDT_InitStruct->WDT_Clock;
	WDT->CON.IE = WDT_InitStruct->WDT_IE;
	WDT->CON.RSTEN = WDT_InitStruct->WDT_Rst;

	return;
}

/*************************************
  函数名：WDT_SetReloadValue
  描  述：设置WDT重载的初值
  输入值：初始值，32-bit无符号整数
  返回值：无
 **************************************/
void WDT_SetReloadValue(uint32_t Value)
{
	WDT->LOAD.Word = Value;
	return;
}

/*************************************
  函数名：WDT_GetValue
  描  述：获取WDT当前值
  输入值：无
  返回值：当前值
 **************************************/
uint32_t WDT_GetValue(void)
{
	return (uint32_t)WDT->VALUE.Word;
}

/*************************************
  函数名：WDT_GetFlagStatus
  描  述：获取WDT中断状态
  输入值：无
  返回值：SET/RESET
 **************************************/
FlagStatus WDT_GetFlagStatus(void)
{
	FlagStatus bitstatus = RESET;

	if (WDT->RIS.WDTIF != RESET)
		bitstatus = SET;

	return  bitstatus;
}
