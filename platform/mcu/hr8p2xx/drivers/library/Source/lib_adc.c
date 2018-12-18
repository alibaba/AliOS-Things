/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#include "lib_adc.h"

void ADC_Init(ADC_InitStruType * ADC_InitStruct)
{

    ADC->CON1.CLKS   = ADC_InitStruct->ADC_ClkS;
    ADC->CON1.CLKDIV = ADC_InitStruct->ADC_ClkDiv;
    ADC->CON1.FM     = ADC_InitStruct->ADC_Align;
    ADC->CON1.VREFP  = ADC_InitStruct->ADC_VrefP;
    ADC->CON1.SMPS   = ADC_InitStruct->ADC_SampS;
    ADC->CON1.ST     = ADC_InitStruct->ADC_SampClk;
    ADC->CON1.HSEN   = ADC_InitStruct->ADC_ConvSpeed;
    ADC->CHS.CHS     = ADC_InitStruct->ADC_ChS;
    ADC->BUF.LP      = ADC_InitStruct->ADC_Lp;
    ADC->BUF.BUF_EN  = ADC_InitStruct->ADC_Buf;
}

void ADC_Set_CH(ADC_TYPE_CHS AdcCH)
{
    ADC->CHS.CHS = AdcCH;
}

uint16_t ADC_GetConvValue(void)
{   
    return ((uint16_t)ADC->DR.DR);
}

FlagStatus ADC_GetConvStatus(void)
{
    FlagStatus bitstatus = RESET;

    if ((ADC->CON0.TRG != (uint32_t)RESET)) {
        bitstatus = SET;
    }
    else {
        bitstatus = RESET;
    }

    return  bitstatus;
}

FlagStatus ADC_GetFlagStatus(void)
{
    FlagStatus bitstatus = RESET;

    if ((ADC->IF.IF != (uint32_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

ITStatus ADC_GetITStatus(void)
{
    ITStatus bitstatus = RESET;

    if ((ADC->IF.IF != (uint32_t)RESET)&&(ADC->IE.IE != (uint32_t)RESET))
    {
        bitstatus = SET;
    }
    else
    {
        bitstatus = RESET;
    }
    return  bitstatus;
}

void ADC_Reset(void)
{

    ADC->CON0.Word = 0x00000000;
    ADC->CON1.Word = 0x00009000;
    ADC->CHS.Word  = 0x00400000;
    ADC->IE.Word   = 0x00000000;
    ADC->IF.IF     = 0x1;
    ADC->BUF.Word  = 0x00000009;
}

/*************************END OF FILE**********************/
