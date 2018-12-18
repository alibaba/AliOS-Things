/*********************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd.
 **********************************************************/
#include "systick.h"

__IO uint32_t TimingDelay;

void SysTickInit(void)
{
    SYSTICK_InitStruType x;
    x.SysTick_ClkSource = SysTick_ClkS_Cpu;
    x.SysTick_Value = 2000;                 //100us
    x.SysTick_ITEnable = ENABLE;
    SysTick_Init(&x);
    SysTick_Disable();
}

void Delay_100us(__IO uint32_t nTime)
{
    TimingDelay = nTime;
    SysTick_Enable();
    while(TimingDelay != 0);
    SysTick_Disable();
}

void Delay1ms(__IO uint32_t nTime)
{
    uint32_t i;

    for(i=0;i<nTime;i++)
        Delay_100us(10);
}

void TimingDelay_Decrement(void)
{
    if(TimingDelay != 0)
        TimingDelay --;
}
