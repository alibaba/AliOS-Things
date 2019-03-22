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
#include "SWM320_pwm.h"

void PWM_Init(PWM_TypeDef * PWMx, PWM_InitStructure * initStruct)
{
    uint32_t bit_offset = 0;

    SYS->CLKEN |= (0x01 << SYS_CLKEN_PWM_Pos);

    SYS->CLKDIV &= ~SYS_CLKDIV_PWM_Msk;
    SYS->CLKDIV |= (initStruct->clk_div << SYS_CLKDIV_PWM_Pos);

    PWM_Stop(PWMx, 1, 1);

    PWMx->MODE = initStruct->mode;

    PWMx->PERA = initStruct->cycleA;
    PWMx->HIGHA = initStruct->hdutyA;
    PWMx->DZA = initStruct->deadzoneA;

    PWMx->PERB = initStruct->cycleB;
    PWMx->HIGHB = initStruct->hdutyB;
    PWMx->DZB = initStruct->deadzoneB;

    PWMx->INIOUT &= ~(PWM_INIOUT_PWMA_Msk | PWM_INIOUT_PWMB_Msk);
    PWMx->INIOUT |= (initStruct->initLevelA << PWM_INIOUT_PWMA_Pos) |
                    (initStruct->initLevelB << PWM_INIOUT_PWMB_Pos);

    PWMG->IM = 0x00000000;

    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        bit_offset = 0;
        break;

    case((uint32_t)PWM1):
        bit_offset = 2;
        break;

    case((uint32_t)PWM2):
        bit_offset = 4;
        break;

    case((uint32_t)PWM3):
        bit_offset = 6;
        break;

    case((uint32_t)PWM4):
        bit_offset = 8;
        break;

    case((uint32_t)PWM5):
        bit_offset = 10;
        break;
    }

    PWMG->IRS = ((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13))); //清除中断标志
    PWMG->IE &= ~((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13)));
    PWMG->IE |= (initStruct->NCycleAIEn << bit_offset) | (initStruct->NCycleBIEn << (bit_offset+1)) |
                (initStruct->HEndAIEn << (bit_offset+12)) | (initStruct->HEndBIEn << (bit_offset+13));

    if(initStruct->NCycleAIEn | initStruct->NCycleBIEn | initStruct->HEndAIEn | initStruct->HEndBIEn)
    {
        NVIC_EnableIRQ(PWM_IRQn);
    }
    else if((PWMG->IE & (~((0x01 << bit_offset) | (0x01 << (bit_offset+1))  | (0x01 << (bit_offset+12)) | (0x01 << (bit_offset+13))))) == 0)
    {
        NVIC_DisableIRQ(PWM_IRQn);
    }
}

void PWM_Start(PWM_TypeDef * PWMx, uint32_t chA, uint32_t chB)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        PWMG->CHEN |= (chA << PWMG_CHEN_PWM0A_Pos) | (chB << PWMG_CHEN_PWM0B_Pos);
        break;

    case((uint32_t)PWM1):
        PWMG->CHEN |= (chA << PWMG_CHEN_PWM1A_Pos) | (chB << PWMG_CHEN_PWM1B_Pos);
        break;

    case((uint32_t)PWM2):
        PWMG->CHEN |= (chA << PWMG_CHEN_PWM2A_Pos) | (chB << PWMG_CHEN_PWM2B_Pos);
        break;

    case((uint32_t)PWM3):
        PWMG->CHEN |= (chA << PWMG_CHEN_PWM3A_Pos) | (chB << PWMG_CHEN_PWM3B_Pos);
        break;

    case((uint32_t)PWM4):
        PWMG->CHEN |= (chA << PWMG_CHEN_PWM4A_Pos) | (chB << PWMG_CHEN_PWM4B_Pos);
        break;

    case((uint32_t)PWM5):
        PWMG->CHEN |= (chA << PWMG_CHEN_PWM5A_Pos) | (chB << PWMG_CHEN_PWM5B_Pos);
        break;
    }
}

void PWM_Stop(PWM_TypeDef * PWMx, uint32_t chA, uint32_t chB)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        PWMG->CHEN &= ~((chA << PWMG_CHEN_PWM0A_Pos) | (chB << PWMG_CHEN_PWM0B_Pos));
        break;

    case((uint32_t)PWM1):
        PWMG->CHEN &= ~((chA << PWMG_CHEN_PWM1A_Pos) | (chB << PWMG_CHEN_PWM1B_Pos));
        break;

    case((uint32_t)PWM2):
        PWMG->CHEN &= ~((chA << PWMG_CHEN_PWM2A_Pos) | (chB << PWMG_CHEN_PWM2B_Pos));
        break;

    case((uint32_t)PWM3):
        PWMG->CHEN &= ~((chA << PWMG_CHEN_PWM3A_Pos) | (chB << PWMG_CHEN_PWM3B_Pos));
        break;

    case((uint32_t)PWM4):
        PWMG->CHEN &= ~((chA << PWMG_CHEN_PWM4A_Pos) | (chB << PWMG_CHEN_PWM4B_Pos));
        break;

    case((uint32_t)PWM5):
        PWMG->CHEN &= ~((chA << PWMG_CHEN_PWM5A_Pos) | (chB << PWMG_CHEN_PWM5B_Pos));
        break;
    }
}

void PWM_SetCycle(PWM_TypeDef * PWMx, uint32_t chn, uint16_t cycle)
{
    if(chn == PWM_CH_A)
        PWMx->PERA = cycle;
    else if(chn == PWM_CH_B)
        PWMx->PERB = cycle;
}

uint16_t PWM_GetCycle(PWM_TypeDef * PWMx, uint32_t chn)
{
    uint16_t cycle = 0;

    if(chn == PWM_CH_A)
        cycle = PWMx->PERA;
    else if(chn == PWM_CH_B)
        cycle = PWMx->PERB;

    return cycle;
}

void PWM_SetHDuty(PWM_TypeDef * PWMx, uint32_t chn, uint16_t hduty)
{
    if(chn == PWM_CH_A)
        PWMx->HIGHA = hduty;
    else if(chn == PWM_CH_B)
        PWMx->HIGHB = hduty;
}

uint16_t PWM_GetHDuty(PWM_TypeDef * PWMx, uint32_t chn)
{
    uint16_t hduty = 0;

    if(chn == PWM_CH_A)
        hduty = PWMx->HIGHA;
    else if(chn == PWM_CH_B)
        hduty = PWMx->HIGHB;

    return hduty;
}

void PWM_SetDeadzone(PWM_TypeDef * PWMx, uint32_t chn, uint8_t deadzone)
{
    if(chn == PWM_CH_A)
        PWMx->DZA = deadzone;
    else if(chn == PWM_CH_B)
        PWMx->DZB = deadzone;
}

uint8_t  PWM_GetDeadzone(PWM_TypeDef * PWMx, uint32_t chn)
{
    uint8_t deadzone = 0;

    if(chn == PWM_CH_A)
        deadzone = PWMx->DZA;
    else if(chn == PWM_CH_B)
        deadzone = PWMx->DZB;

    return deadzone;
}

void PWM_IntNCycleEn(PWM_TypeDef * PWMx, uint32_t chn)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_NEWP0A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_NEWP0B_Pos);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_NEWP1A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_NEWP1B_Pos);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_NEWP2A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_NEWP2B_Pos);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_NEWP3A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_NEWP3B_Pos);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_NEWP4A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_NEWP4B_Pos);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_NEWP5A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_NEWP5B_Pos);
        break;
    }
}

void PWM_IntNCycleDis(PWM_TypeDef * PWMx, uint32_t chn)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_NEWP0A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_NEWP0B_Pos);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_NEWP1A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_NEWP1B_Pos);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_NEWP2A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_NEWP2B_Pos);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_NEWP3A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_NEWP3B_Pos);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_NEWP4A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_NEWP4B_Pos);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_NEWP5A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_NEWP5B_Pos);
        break;
    }
}

void PWM_IntNCycleClr(PWM_TypeDef * PWMx, uint32_t chn)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_NEWP0A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_NEWP0B_Pos);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_NEWP1A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_NEWP1B_Pos);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_NEWP2A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_NEWP2B_Pos);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_NEWP3A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_NEWP3B_Pos);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_NEWP4A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_NEWP4B_Pos);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_NEWP5A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_NEWP5B_Pos);
        break;
    }
}

uint32_t PWM_IntNCycleStat(PWM_TypeDef * PWMx, uint32_t chn)
{
    uint32_t int_stat = 0;

    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_NEWP0A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_NEWP0B_Msk);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_NEWP1A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_NEWP1B_Msk);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_NEWP2A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_NEWP2B_Msk);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_NEWP3A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_NEWP3B_Msk);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_NEWP4A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_NEWP4B_Msk);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_NEWP5A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_NEWP5B_Msk);
        break;
    }

    return int_stat;
}

void PWM_IntHEndEn(PWM_TypeDef * PWMx, uint32_t chn)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_HEND0A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_HEND0B_Pos);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_HEND1A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_HEND1B_Pos);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_HEND2A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_HEND2B_Pos);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_HEND3A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_HEND3B_Pos);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_HEND4A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_HEND4B_Pos);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) PWMG->IE |= (0x01 << PWMG_IE_HEND5A_Pos);
        else                PWMG->IE |= (0x01 << PWMG_IE_HEND5B_Pos);
        break;
    }
}

void PWM_IntHEndDis(PWM_TypeDef * PWMx, uint32_t chn)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_HEND0A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_HEND0B_Pos);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_HEND1A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_HEND1B_Pos);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_HEND2A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_HEND2B_Pos);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_HEND3A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_HEND3B_Pos);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_HEND4A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_HEND4B_Pos);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) PWMG->IE &= ~(0x01 << PWMG_IE_HEND5A_Pos);
        else                PWMG->IE &= ~(0x01 << PWMG_IE_HEND5B_Pos);
        break;
    }
}

void PWM_IntHEndClr(PWM_TypeDef * PWMx, uint32_t chn)
{
    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_HEND0A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_HEND0B_Pos);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_HEND1A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_HEND1B_Pos);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_HEND2A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_HEND2B_Pos);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_HEND3A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_HEND3B_Pos);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_HEND4A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_HEND4B_Pos);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) PWMG->IRS = (0x01 << PWMG_IRS_HEND5A_Pos);
        else                PWMG->IRS = (0x01 << PWMG_IRS_HEND5B_Pos);
        break;
    }
}

uint32_t PWM_IntHEndStat(PWM_TypeDef * PWMx, uint32_t chn)
{
    uint32_t int_stat = 0;

    switch((uint32_t)PWMx)
    {
    case((uint32_t)PWM0):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_HEND0A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_HEND0B_Msk);
        break;

    case((uint32_t)PWM1):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_HEND1A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_HEND1B_Msk);
        break;

    case((uint32_t)PWM2):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_HEND2A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_HEND2B_Msk);
        break;

    case((uint32_t)PWM3):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_HEND3A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_HEND3B_Msk);
        break;

    case((uint32_t)PWM4):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_HEND4A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_HEND4B_Msk);
        break;

    case((uint32_t)PWM5):
        if(chn == PWM_CH_A) int_stat = (PWMG->IF & PWMG_IF_HEND5A_Msk);
        else                int_stat = (PWMG->IF & PWMG_IF_HEND5B_Msk);
        break;
    }

    return int_stat;
}
