/******************************************************************************************************************************************
* @attention
*
* THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS WITH CODING INFORMATION
* REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE TIME. AS A RESULT, SYNWIT SHALL NOT BE HELD LIABLE
* FOR ANY DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
* OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION CONTAINED HEREIN IN CONN-
* -ECTION WITH THEIR PRODUCTS.
*
* COPYRIGHT 2012 Synwit Technology
*******************************************************************************************************************************************/

#include "SWM320.h"
#include "SWM320_wdt.h"

void WDT_Init(WDT_TypeDef * WDTx, uint32_t peroid, uint32_t mode)
{
    SYS->CLKEN |= (0x01 << SYS_CLKEN_WDT_Pos);

    WDT_Stop(WDTx);

    WDTx->LOAD = peroid;

    if(mode == WDT_MODE_RESET)
    {
        NVIC_DisableIRQ(WDT_IRQn);

        WDTx->CR |= (1 << WDT_CR_RSTEN_Pos);
    }
    else //mode == WDT_MODE_INTERRUPT
    {
        NVIC_EnableIRQ(WDT_IRQn);

        WDTx->CR &= ~(1 << WDT_CR_RSTEN_Pos);
    }
}

void WDT_Start(WDT_TypeDef * WDTx)
{
    WDTx->CR |= (0x01 << WDT_CR_EN_Pos);
}

void WDT_Stop(WDT_TypeDef * WDTx)
{
    WDTx->CR &= ~(0x01 << WDT_CR_EN_Pos);
}

void WDT_Feed(WDT_TypeDef * WDTx)
{
    WDTx->FEED = 0x55;
}

int32_t WDT_GetValue(WDT_TypeDef * WDTx)
{
    return WDTx->VALUE;
}

void WDT_INTClr(WDT_TypeDef * WDTx)
{
    WDTx->IF = 1;
}

uint32_t WDT_INTStat(WDT_TypeDef * WDTx)
{
    return WDTx->IF;
}
