/*********************************************************
*Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
**********************************************************/
#include "lib_wdt.h"


enum {
    WDT_PCLK = 0x0,
    WDT_LRC  = 0x1,
};

void WWDT_Init(uint32_t load, uint8_t win, uint8_t it_flag)
{
//  NVIC_Init(WWDG_IRQn, 1, 1, (TYPE_FUNCEN)it_flag);
    WWDT_Unlock();
    WWDT->LOAD.Word = load;

    WWDT->CON.WWDTWIN = win;
    WWDT->CON.CLKS    = 0;
    WWDT->CON.RSTEN   = ENABLE;
    WWDT->CON.IE      = it_flag;
    WWDT->CON.EN      = ENABLE;
    WWDT_Lock();

    return;
}

uint32_t WWDT_GetValue(void)
{
    return WWDT->VALUE.Word;
}

FlagStatus WWDT_GetFlagStatus(void)
{
    return (FlagStatus)(WWDT->RIS.WDTIF);
}

void WWDT_ClearFlag(void)
{
    WWDT_Unlock();
    WWDT->INTCLR.INTCLR = 0x1;
    WWDT_Lock();
    return;
}

void IWDT_Init(uint32_t load, uint8_t it_flag)
{
    //NVIC_Init(IWDG_IRQn, 1, 1, (TYPE_FUNCEN)it_flag);
    IWDT_Unlock();
    IWDT->LOAD.Word = load;

    IWDT->CON.CLKS    = 1;
    IWDT->CON.RSTEN   = 0;
    IWDT->CON.IE      = it_flag;
    IWDT->CON.EN      = ENABLE;
    IWDT_Lock();

    return;
}

uint32_t IWDT_GetValue(void)
{
    return IWDT->VALUE.Word;
}

FlagStatus IWDT_GetFlagStatus(void)
{
    return (FlagStatus)(IWDT->RIS.WDTIF);
}

void IWDT_ClearFlag(void)
{
    IWDT_Unlock();
    IWDT->INTCLR.INTCLR = 0x1;
    IWDT_Lock();
    return;
}
