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
#include "SWM320_timr.h"

void TIMR_Init(TIMR_TypeDef * TIMRx, uint32_t mode, uint32_t period, uint32_t int_en)
{
    SYS->CLKEN |= (0x01 << SYS_CLKEN_TIMR_Pos);

    TIMR_Stop(TIMRx);

    TIMRx->CTRL &= ~TIMR_CTRL_CLKSRC_Msk;
    TIMRx->CTRL |= mode << TIMR_CTRL_CLKSRC_Pos;

    TIMRx->LDVAL = period;

    switch((uint32_t)TIMRx)
    {
    case ((uint32_t)TIMR0):
        TIMRG->IF = (1 << TIMRG_IF_TIMR0_Pos);
        TIMRG->IE &= ~TIMRG_IE_TIMR0_Msk;
        TIMRG->IE |= (int_en << TIMRG_IE_TIMR0_Pos);

        if(int_en) NVIC_EnableIRQ(TIMR0_IRQn);
        break;

    case ((uint32_t)TIMR1):
        TIMRG->IF = (1 << TIMRG_IF_TIMR1_Pos);
        TIMRG->IE &= ~TIMRG_IE_TIMR1_Msk;
        TIMRG->IE |= (int_en << TIMRG_IE_TIMR1_Pos);

        if(int_en) NVIC_EnableIRQ(TIMR1_IRQn);
        break;

    case ((uint32_t)TIMR2):
        TIMRG->IF = (1 << TIMRG_IF_TIMR2_Pos);
        TIMRG->IE &= ~TIMRG_IE_TIMR2_Msk;
        TIMRG->IE |= (int_en << TIMRG_IE_TIMR2_Pos);

        if(int_en) NVIC_EnableIRQ(TIMR2_IRQn);
        break;

    case ((uint32_t)TIMR3):
        TIMRG->IF = (1 << TIMRG_IF_TIMR3_Pos);
        TIMRG->IE &= ~TIMRG_IE_TIMR3_Msk;
        TIMRG->IE |= (int_en << TIMRG_IE_TIMR3_Pos);

        if(int_en) NVIC_EnableIRQ(TIMR3_IRQn);
        break;

    case ((uint32_t)TIMR4):
        TIMRG->IF = (1 << TIMRG_IF_TIMR4_Pos);
        TIMRG->IE &= ~TIMRG_IE_TIMR4_Msk;
        TIMRG->IE |= (int_en << TIMRG_IE_TIMR4_Pos);

        if(int_en) NVIC_EnableIRQ(TIMR4_IRQn);
        break;

    case ((uint32_t)TIMR5):
        TIMRG->IF = (1 << TIMRG_IF_TIMR5_Pos);
        TIMRG->IE &= ~TIMRG_IE_TIMR5_Msk;
        TIMRG->IE |= (int_en << TIMRG_IE_TIMR5_Pos);

        if(int_en) NVIC_EnableIRQ(TIMR5_IRQn);
        break;
    }
}

void TIMR_Start(TIMR_TypeDef * TIMRx)
{
    TIMRx->CTRL |= TIMR_CTRL_EN_Msk;
}

void TIMR_Stop(TIMR_TypeDef * TIMRx)
{
    TIMRx->CTRL &= ~TIMR_CTRL_EN_Msk;
}

void TIMR_Halt(TIMR_TypeDef * TIMRx)
{
    switch((uint32_t)TIMRx)
    {
    case ((uint32_t)TIMR0):
        TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR0_Pos);
        break;

    case ((uint32_t)TIMR1):
        TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR1_Pos);
        break;

    case ((uint32_t)TIMR2):
        TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR2_Pos);
        break;

    case ((uint32_t)TIMR3):
        TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR3_Pos);
        break;

    case ((uint32_t)TIMR4):
        TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR4_Pos);
        break;

    case ((uint32_t)TIMR5):
        TIMRG->HALT |= (0x01 << TIMRG_HALT_TIMR5_Pos);
        break;
    }
}

void TIMR_Resume(TIMR_TypeDef * TIMRx)
{
    switch((uint32_t)TIMRx)
    {
    case ((uint32_t)TIMR0):
        TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR0_Pos);
        break;

    case ((uint32_t)TIMR1):
        TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR1_Pos);
        break;

    case ((uint32_t)TIMR2):
        TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR2_Pos);
        break;

    case ((uint32_t)TIMR3):
        TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR3_Pos);
        break;

    case ((uint32_t)TIMR4):
        TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR4_Pos);
        break;

    case ((uint32_t)TIMR5):
        TIMRG->HALT &= ~(0x01 << TIMRG_HALT_TIMR5_Pos);
        break;
    }
}

void TIMR_SetPeriod(TIMR_TypeDef * TIMRx, uint32_t period)
{
    TIMRx->LDVAL = period;
}

uint32_t TIMR_GetPeriod(TIMR_TypeDef * TIMRx)
{
    return TIMRx->LDVAL;
}

uint32_t TIMR_GetCurValue(TIMR_TypeDef * TIMRx)
{
    return TIMRx->CVAL;
}

void TIMR_INTEn(TIMR_TypeDef * TIMRx)
{
    switch((uint32_t)TIMRx)
    {
    case ((uint32_t)TIMR0):
        TIMRG->IE |= (0x01 << TIMRG_IE_TIMR0_Pos);
        NVIC_EnableIRQ(TIMR0_IRQn);
        break;

    case ((uint32_t)TIMR1):
        TIMRG->IE |= (0x01 << TIMRG_IE_TIMR1_Pos);
        NVIC_EnableIRQ(TIMR1_IRQn);
        break;

    case ((uint32_t)TIMR2):
        TIMRG->IE |= (0x01 << TIMRG_IE_TIMR2_Pos);
        NVIC_EnableIRQ(TIMR2_IRQn);
        break;

    case ((uint32_t)TIMR3):
        TIMRG->IE |= (0x01 << TIMRG_IE_TIMR3_Pos);
        NVIC_EnableIRQ(TIMR3_IRQn);
        break;

    case ((uint32_t)TIMR4):
        TIMRG->IE |= (0x01 << TIMRG_IE_TIMR4_Pos);
        NVIC_EnableIRQ(TIMR4_IRQn);
        break;

    case ((uint32_t)TIMR5):
        TIMRG->IE |= (0x01 << TIMRG_IE_TIMR5_Pos);
        NVIC_EnableIRQ(TIMR5_IRQn);
        break;
    }
}

void TIMR_INTDis(TIMR_TypeDef * TIMRx)
{
    switch((uint32_t)TIMRx)
    {
    case ((uint32_t)TIMR0):
        TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR0_Pos);
        break;

    case ((uint32_t)TIMR1):
        TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR1_Pos);
        break;

    case ((uint32_t)TIMR2):
        TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR2_Pos);
        break;

    case ((uint32_t)TIMR3):
        TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR3_Pos);
        break;

    case ((uint32_t)TIMR4):
        TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR4_Pos);
        break;

    case ((uint32_t)TIMR5):
        TIMRG->IE &= ~(0x01 << TIMRG_IE_TIMR5_Pos);
        break;
    }
}

void TIMR_INTClr(TIMR_TypeDef * TIMRx)
{
    switch((uint32_t)TIMRx)
    {
    case ((uint32_t)TIMR0):
        TIMRG->IF = (0x01 << TIMRG_IF_TIMR0_Pos);
        break;

    case ((uint32_t)TIMR1):
        TIMRG->IF = (0x01 << TIMRG_IF_TIMR1_Pos);
        break;

    case ((uint32_t)TIMR2):
        TIMRG->IF = (0x01 << TIMRG_IF_TIMR2_Pos);
        break;

    case ((uint32_t)TIMR3):
        TIMRG->IF = (0x01 << TIMRG_IF_TIMR3_Pos);
        break;

    case ((uint32_t)TIMR4):
        TIMRG->IF = (0x01 << TIMRG_IF_TIMR4_Pos);
        break;

    case ((uint32_t)TIMR5):
        TIMRG->IF = (0x01 << TIMRG_IF_TIMR5_Pos);
        break;
    }
}

uint32_t TIMR_INTStat(TIMR_TypeDef * TIMRx)
{
    switch((uint32_t)TIMRx)
    {
    case ((uint32_t)TIMR0):
        return (TIMRG->IF & TIMRG_IF_TIMR0_Msk) ? 1 : 0;

    case ((uint32_t)TIMR1):
        return (TIMRG->IF & TIMRG_IF_TIMR1_Msk) ? 1 : 0;

    case ((uint32_t)TIMR2):
        return (TIMRG->IF & TIMRG_IF_TIMR2_Msk) ? 1 : 0;

    case ((uint32_t)TIMR3):
        return (TIMRG->IF & TIMRG_IF_TIMR3_Msk) ? 1 : 0;

    case ((uint32_t)TIMR4):
        return (TIMRG->IF & TIMRG_IF_TIMR4_Msk) ? 1 : 0;

    case ((uint32_t)TIMR5):
        return (TIMRG->IF & TIMRG_IF_TIMR5_Msk) ? 1 : 0;
    }

    return 0;
}
