/*******************************************************************************************************************************************
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
#include "SWM320_adc.h"

void ADC_Init(ADC_TypeDef * ADCx, ADC_InitStructure * initStruct)
{
    switch((uint32_t)ADCx)
    {
    case ((uint32_t)ADC0):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_ADC0_Pos);
        break;

    case ((uint32_t)ADC1):
        SYS->CLKEN |= (0x01 << SYS_CLKEN_ADC1_Pos);
        break;
    }

    ADC_Close(ADCx);

    if(initStruct->clk_src == ADC_CLKSRC_HRC)
    {
        ADCx->CTRL |= (1 << ADC_CTRL_CLKSRC_Pos);

        ADCx->CTRL2 &= ~ADC_CTRL2_CLKDIV_Msk;
        ADCx->CTRL2 |= (initStruct->clk_div << ADC_CTRL2_CLKDIV_Pos);
    }
    else
    {
        if(SYS->PLLCR & SYS_PLLCR_OFF_Msk) PLLInit();

        ADCx->CTRL &= ~(1 << ADC_CTRL_CLKSRC_Pos);

        SYS->PLLDIV &= ~SYS_PLLDIV_ADVCO_Msk;
        SYS->PLLDIV |= ((initStruct->clk_src - 2) << SYS_PLLDIV_ADVCO_Pos);

        SYS->PLLDIV &= ~SYS_PLLDIV_ADDIV_Msk;
        SYS->PLLDIV |= (initStruct->clk_div << SYS_PLLDIV_ADDIV_Pos);
    }

    ADCx->CALIBSET = (ADCx == ADC0) ? SYS->BKP[0] : SYS->BKP[1];
    ADCx->CALIBEN  = (1 << ADC_CALIBEN_OFFSET_Pos) | (1 << ADC_CALIBEN_K_Pos);

    ADCx->CTRL2 &= ~(ADC_CTRL2_ADCEVCM_Msk | ADC_CTRL2_PGAIVCM_Msk | ADC_CTRL2_PGAGAIN_Msk | ADC_CTRL2_PGAVCM_Msk);
    ADCx->CTRL2 |= (0                    << ADC_CTRL2_ADCEVCM_Pos) |
                   (initStruct->pga_ref  << ADC_CTRL2_PGAIVCM_Pos) |
                   (6                    << ADC_CTRL2_PGAGAIN_Pos) |
                   ((uint32_t)6          << ADC_CTRL2_PGAVCM_Pos);

    ADCx->CTRL &= ~(0xFF                << ADC_CTRL_CH0_Pos);
    ADCx->CTRL |= (initStruct->channels << ADC_CTRL_CH0_Pos);

    ADCx->CTRL &= ~(ADC_CTRL_AVG_Msk | ADC_CTRL_TRIG_Msk | ADC_CTRL_CONT_Msk);
    ADCx->CTRL |= (initStruct->samplAvg << ADC_CTRL_AVG_Pos)  |
                  (initStruct->trig_src << ADC_CTRL_TRIG_Pos) |
                  (initStruct->Continue << ADC_CTRL_CONT_Pos);

    ADCx->IF = 0xFFFFFFFF;

    ADCx->IE &= ~(ADC_IE_CH0EOC_Msk | ADC_IE_CH1EOC_Msk | ADC_IE_CH2EOC_Msk | ADC_IE_CH3EOC_Msk |
                  ADC_IE_CH4EOC_Msk | ADC_IE_CH5EOC_Msk | ADC_IE_CH6EOC_Msk | ADC_IE_CH7EOC_Msk);
    ADCx->IE |= (((initStruct->EOC_IEn & ADC_CH0) ? 1 : 0) << ADC_IE_CH0EOC_Pos) |
                (((initStruct->EOC_IEn & ADC_CH1) ? 1 : 0) << ADC_IE_CH1EOC_Pos) |
                (((initStruct->EOC_IEn & ADC_CH2) ? 1 : 0) << ADC_IE_CH2EOC_Pos) |
                (((initStruct->EOC_IEn & ADC_CH3) ? 1 : 0) << ADC_IE_CH3EOC_Pos) |
                (((initStruct->EOC_IEn & ADC_CH4) ? 1 : 0) << ADC_IE_CH4EOC_Pos) |
                (((initStruct->EOC_IEn & ADC_CH5) ? 1 : 0) << ADC_IE_CH5EOC_Pos) |
                (((initStruct->EOC_IEn & ADC_CH6) ? 1 : 0) << ADC_IE_CH6EOC_Pos) |
                (((initStruct->EOC_IEn & ADC_CH7) ? 1 : 0) << ADC_IE_CH7EOC_Pos);

    ADCx->IE &= ~(ADC_IE_CH0OVF_Msk | ADC_IE_CH1OVF_Msk | ADC_IE_CH2OVF_Msk | ADC_IE_CH3OVF_Msk |
                  ADC_IE_CH4OVF_Msk | ADC_IE_CH5OVF_Msk | ADC_IE_CH6OVF_Msk | ADC_IE_CH7OVF_Msk);
    ADCx->IE |= (((initStruct->OVF_IEn & ADC_CH0) ? 1 : 0) << ADC_IE_CH0OVF_Pos) |
                (((initStruct->OVF_IEn & ADC_CH1) ? 1 : 0) << ADC_IE_CH1OVF_Pos) |
                (((initStruct->OVF_IEn & ADC_CH2) ? 1 : 0) << ADC_IE_CH2OVF_Pos) |
                (((initStruct->OVF_IEn & ADC_CH3) ? 1 : 0) << ADC_IE_CH3OVF_Pos) |
                (((initStruct->OVF_IEn & ADC_CH4) ? 1 : 0) << ADC_IE_CH4OVF_Pos) |
                (((initStruct->OVF_IEn & ADC_CH5) ? 1 : 0) << ADC_IE_CH5OVF_Pos) |
                (((initStruct->OVF_IEn & ADC_CH6) ? 1 : 0) << ADC_IE_CH6OVF_Pos) |
                (((initStruct->OVF_IEn & ADC_CH7) ? 1 : 0) << ADC_IE_CH7OVF_Pos);

    ADCx->IE &= ~(ADC_IE_CH0HFULL_Msk | ADC_IE_CH1HFULL_Msk | ADC_IE_CH2HFULL_Msk | ADC_IE_CH3HFULL_Msk |
                  ADC_IE_CH4HFULL_Msk | ADC_IE_CH5HFULL_Msk | ADC_IE_CH6HFULL_Msk | ADC_IE_CH7HFULL_Msk);
    ADCx->IE |= (((initStruct->HFULL_IEn & ADC_CH0) ? 1 : 0) << ADC_IE_CH0HFULL_Pos) |
                (((initStruct->HFULL_IEn & ADC_CH1) ? 1 : 0) << ADC_IE_CH1HFULL_Pos) |
                (((initStruct->HFULL_IEn & ADC_CH2) ? 1 : 0) << ADC_IE_CH2HFULL_Pos) |
                (((initStruct->HFULL_IEn & ADC_CH3) ? 1 : 0) << ADC_IE_CH3HFULL_Pos) |
                (((initStruct->HFULL_IEn & ADC_CH4) ? 1 : 0) << ADC_IE_CH4HFULL_Pos) |
                (((initStruct->HFULL_IEn & ADC_CH5) ? 1 : 0) << ADC_IE_CH5HFULL_Pos) |
                (((initStruct->HFULL_IEn & ADC_CH6) ? 1 : 0) << ADC_IE_CH6HFULL_Pos) |
                (((initStruct->HFULL_IEn & ADC_CH7) ? 1 : 0) << ADC_IE_CH7HFULL_Pos);

    ADCx->IE &= ~(uint32_t)(ADC_IE_CH0FULL_Msk | ADC_IE_CH1FULL_Msk | ADC_IE_CH2FULL_Msk | ADC_IE_CH3FULL_Msk |
                  ADC_IE_CH4FULL_Msk | ADC_IE_CH5FULL_Msk | ADC_IE_CH6FULL_Msk | ADC_IE_CH7FULL_Msk);
    ADCx->IE |= (((initStruct->FULL_IEn & ADC_CH0) ? 1 : 0) << ADC_IE_CH0FULL_Pos) |
                (((initStruct->FULL_IEn & ADC_CH1) ? 1 : 0) << ADC_IE_CH1FULL_Pos) |
                (((initStruct->FULL_IEn & ADC_CH2) ? 1 : 0) << ADC_IE_CH2FULL_Pos) |
                (((initStruct->FULL_IEn & ADC_CH3) ? 1 : 0) << ADC_IE_CH3FULL_Pos) |
                (((initStruct->FULL_IEn & ADC_CH4) ? 1 : 0) << ADC_IE_CH4FULL_Pos) |
                (((initStruct->FULL_IEn & ADC_CH5) ? 1 : 0) << ADC_IE_CH5FULL_Pos) |
                (((initStruct->FULL_IEn & ADC_CH6) ? 1 : 0) << ADC_IE_CH6FULL_Pos) |
                (((initStruct->FULL_IEn & ADC_CH7) ? 1 : 0) << ADC_IE_CH7FULL_Pos);

    switch((uint32_t)ADCx)
    {
    case ((uint32_t)ADC0):
        if(initStruct->EOC_IEn | initStruct->OVF_IEn | initStruct->HFULL_IEn | initStruct->FULL_IEn)
        {
            NVIC_EnableIRQ(ADC0_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(ADC0_IRQn);
        }
        break;

    case ((uint32_t)ADC1):
        if(initStruct->EOC_IEn | initStruct->OVF_IEn | initStruct->HFULL_IEn | initStruct->FULL_IEn)
        {
            NVIC_EnableIRQ(ADC1_IRQn);
        }
        else
        {
            NVIC_DisableIRQ(ADC1_IRQn);
        }
        break;
    }
}

void ADC_Open(ADC_TypeDef * ADCx)
{
    ADCx->CTRL |= (0x01 << ADC_CTRL_EN_Pos);
}

void ADC_Close(ADC_TypeDef * ADCx)
{
    ADCx->CTRL &= ~(0x01 << ADC_CTRL_EN_Pos);
}

void ADC_Start(ADC_TypeDef * ADCx)
{
    ADCx->START |= (0x01 << ADC_START_GO_Pos);
}

void ADC_Stop(ADC_TypeDef * ADCx)
{
    ADCx->START &= ~(0x01 << ADC_START_GO_Pos);
}

static uint32_t chn2idx(uint32_t chn)
{
    uint32_t idx = 0;

    switch(chn)
    {
        case 0x01: idx = 0; break;
        case 0x02: idx = 1; break;
        case 0x04: idx = 2; break;
        case 0x08: idx = 3; break;
        case 0x10: idx = 4; break;
        case 0x20: idx = 5; break;
        case 0x40: idx = 6; break;
        case 0x80: idx = 7; break;
    }

    return idx;
}

uint32_t ADC_Read(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t dat = 0;
    uint32_t idx = chn2idx(chn);

    dat = ADCx->CH[idx].DATA;

    ADCx->CH[idx].STAT = 0x01;

    return dat;
}

uint32_t ADC_IsEOC(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    return (ADCx->CH[idx].STAT & ADC_STAT_EOC_Msk) ? 1 : 0;
}

void ADC_ChnSelect(ADC_TypeDef * ADCx, uint32_t chns)
{
    ADCx->CTRL &= ~(0xFF << ADC_CTRL_CH0_Pos);
    ADCx->CTRL |=  (chns << ADC_CTRL_CH0_Pos);
}

void ADC_IntEOCEn(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE |= (0x01 << (idx*4));
}

void ADC_IntEOCDis(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE &= ~(0x01 << (idx*4));
}

void ADC_IntEOCClr(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IF = (0x01 << (idx*4));
}

uint32_t ADC_IntEOCStat(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    return (ADCx->IF & (0x01 << (idx*4))) ? 1 : 0;
}

void ADC_IntOVFEn(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE |= (0x01 << (idx*4+1));
}

void ADC_IntOVFDis(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE &= ~(0x01 << (idx*4+1));
}

void ADC_IntOVFClr(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IF = (0x01 << (idx*4+1));
}

uint32_t ADC_IntOVFStat(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    return (ADCx->IF & (0x01 << (idx*4+1))) ? 1 : 0;
}

void ADC_IntHFULLEn(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE |= (0x01 << (idx*4+2));
}

void ADC_IntHFULLDis(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE &= ~(0x01 << (idx*4+2));
}

void ADC_IntHFULLClr(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IF = (0x01 << (idx*4+2));
}

uint32_t ADC_IntHFULLStat(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    return (ADCx->IF & (0x01 << (idx*4+2))) ? 1 : 0;
}

void ADC_IntFULLEn(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE |= (0x01 << (idx*4+3));
}

void ADC_IntFULLDis(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IE &= ~(0x01 << (idx*4+3));
}

void ADC_IntFULLClr(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    ADCx->IF = (0x01 << (idx*4+3));
}

uint32_t ADC_IntFULLStat(ADC_TypeDef * ADCx, uint32_t chn)
{
    uint32_t idx = chn2idx(chn);

    return (ADCx->IF & (0x01 << (idx*4+3))) ? 1 : 0;
}
