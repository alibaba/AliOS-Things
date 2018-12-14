/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#include "lib_scs.h"

void NVIC_Init(NVIC_IRQChannel Channel,NVIC_IRQPriority Priority,TYPE_FUNCEN Cmd)
{
    uint32_t tmppriority = 0x00;

    if (Cmd != DISABLE)
    {
        /* Compute the Corresponding IRQ Priority */    
        tmppriority = NVIC->IP[Channel >> 0x02];
        tmppriority &= (uint32_t)(~(((uint32_t)0xFF) << ((Channel & 0x03) * 8)));
        tmppriority |= (uint32_t)((((uint32_t)Priority << 6) & 0xFF) << ((Channel & 0x03) * 8));    

        NVIC->IP[Channel >> 0x02] = tmppriority;

        /* Enable the Selected IRQ Channels */
        NVIC->ISER[0] = (uint32_t)0x01 << (Channel & (uint8_t)0x1F);
    }
    else
    {
        /* Disable the Selected IRQ Channels */
        NVIC->ICER[0] = (uint32_t)0x01 << (Channel & (uint8_t)0x1F);
    }
}

void SCB_SystemLPConfig(SCB_TYPE_SCR LowPowerMode, TYPE_FUNCEN NewState)
{
    if (NewState != DISABLE)
        SCB->SCR |= LowPowerMode;
    else
        SCB->SCR &= (uint32_t)(~(uint32_t)LowPowerMode);
}

uint32_t SCB_GetCpuID(void)
{
    return (SCB->CPUID);
}

void SysTick_Init(SYSTICK_InitStruType* SysT_InitStruct)
{
    uint32_t temp32 = 0;

    SysTick->LOAD = SysT_InitStruct->SysTick_Value;
    SysTick->VAL = (uint32_t)0;

    if(SysT_InitStruct->SysTick_ClkSource != SysTick_ClkS_Base)
        temp32 |= 0x00000004;
    else
        temp32 &= 0xFFFFFFFB;

    if(SysT_InitStruct->SysTick_ITEnable != DISABLE)
        temp32 |= 0x00000002;
    else
        temp32 &= 0xFFFFFFFD;

    SysTick->CTRL = temp32;
}

/*************************END OF FILE**********************/
