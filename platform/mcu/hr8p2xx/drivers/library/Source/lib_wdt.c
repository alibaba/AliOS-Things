/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_wdt.h"
#include "system_HR8P2xx.h"

void WDT_Init(WDT_InitStruType *WDT_InitStruct)
{
    uint32_t tmp;

    if (WDT_InitStruct->WDT_Clock != WDT_CLOCK_PCLK)
        tmp = (uint32_t)(32 * WDT_InitStruct->WDT_Tms);
    else
        tmp = SystemCoreClock / 1000 * WDT_InitStruct->WDT_Tms;

    WDT->LOAD.LOAD=tmp;
    WDT->CON.CLKS = WDT_InitStruct->WDT_Clock;
    WDT->CON.IE = WDT_InitStruct->WDT_IE;
    WDT->CON.RSTEN = WDT_InitStruct->WDT_Rst;

    return;
}

void WDT_SetReloadValue(uint32_t Value)
{
    WDT->LOAD.LOAD = Value;
    return;
}

uint32_t WDT_GetValue(void)
{
    return (uint32_t)WDT->VALUE.VALUE;

}

FlagStatus WDT_GetFlagStatus(void)
{
    FlagStatus bitstatus = RESET;

    if (WDT->RIS.WDTIF != RESET)
        bitstatus = SET;

    return  bitstatus;
}
