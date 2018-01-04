/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#include "lib_wdt.h"

void IWDT_Init(IWDT_InitStruType *WDT_InitStruct)
{
    uint32_t tmp;

    if (WDT_InitStruct->WDT_Clock != WDT_CLOCK_PCLK)
        tmp = (uint32_t)(32 * WDT_InitStruct->WDT_Tms);
    else
        tmp = SystemCoreClock / 1000 * WDT_InitStruct->WDT_Tms;

    IWDT->LOAD.LOAD = tmp;
    IWDT->CON.CLKS = WDT_InitStruct->WDT_Clock;
    IWDT->CON.IE = WDT_InitStruct->WDT_IE;
    IWDT->CON.RSTEN = WDT_InitStruct->WDT_Rst;

    return;
}

void IWDT_SetReloadValue(uint32_t Value)
{
    IWDT->LOAD.LOAD = Value;
    return;
}

uint32_t IWDT_GetValue(void)
{
    return (uint32_t)IWDT->VALUE.VALUE;
}

FlagStatus IWDT_GetFlagStatus(void)
{
    FlagStatus bitstatus = RESET;

    if (IWDT->RIS.WDTIF != RESET)
        bitstatus = SET;

    return  bitstatus;
}

FlagStatus IWDT_GetITStatus(void)
{
    FlagStatus bitstatus = RESET;

    if (IWDT->CON.IE != RESET)
        bitstatus = SET;

    return  bitstatus;
}

void WWDT_Init(WWDT_InitStruType *WDT_InitStruct)
{
    uint32_t tmp;

    if (WDT_InitStruct->WDT_Clock != WDT_CLOCK_PCLK)
        tmp = (uint32_t)(32 * WDT_InitStruct->WDT_Tms);
    else
        tmp = SystemCoreClock / 1000 * WDT_InitStruct->WDT_Tms;

    WWDT->LOAD.LOAD = tmp;
    WWDT->CON.CLKS = WDT_InitStruct->WDT_Clock;
    WWDT->CON.IE = WDT_InitStruct->WDT_IE;
    WWDT->CON.RSTEN = WDT_InitStruct->WDT_Rst;
    WWDT->CON.WWDTWIN = WDT_InitStruct->WDT_Win;

    return;
}

void WWDT_SetReloadValue(uint32_t Value)
{
    WWDT->LOAD.LOAD = Value;
    return;
}

uint32_t WWDT_GetValue(void)
{
    return (uint32_t)WWDT->VALUE.VALUE;
}

FlagStatus WWDT_GetFlagStatus(void)
{
    FlagStatus bitstatus = RESET;

    if (WWDT->RIS.WWDTIF != RESET)
        bitstatus = SET;

    return  bitstatus;
}

FlagStatus WWDT_GetITStatus(void)
{
    FlagStatus bitstatus = RESET;

    if (WWDT->CON.IE != RESET)
        bitstatus = SET;

    return  bitstatus;
}

/*************************END OF FILE**********************/


