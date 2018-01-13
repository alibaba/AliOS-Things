/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd.
 ***************************************************************/
#include "lib_adc.h"

void ADC_Init(ADC_InitStruType * ADC_InitStruct)
{
    ADC->CON1.CLKS = ADC_InitStruct->ADC_ClkS;     
    ADC->CON1.CLKDIV = ADC_InitStruct->ADC_ClkDiv; 
    ADC->CON1.VREFP = ADC_InitStruct->ADC_VrefP;   
    ADC->CON1.SMPS = ADC_InitStruct->ADC_SampS;    
    ADC->CON1.ST = ADC_InitStruct->ADC_SampClk;   
    ADC->CON1.HSEN = ADC_InitStruct->ADC_ConvSpeed;
    ADC->CHS.CHS = ADC_InitStruct->ADC_ChS;        

    ADC->VREFCON.IREF_EN = ADC_InitStruct->ADC_IREF_EN;             
    ADC->VREFCON.VREF_EN = ADC_InitStruct->ADC_VREF_EN;             
    ADC->CON1.VCMBUF_EN = ADC_InitStruct->ADC_VCMBUF_EN;            
    ADC->CON1.VREFN = ADC_InitStruct->ADC_VREFN;                    
    ADC->CON1.VRBUF_EN = ADC_InitStruct->ADC_VRBUF_EN;              
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

    if ((ADC->CON0.TRIG != (uint32_t)RESET))
        bitstatus = SET;                    
    else
        bitstatus = RESET;                  

    return  bitstatus;
}

ErrorStatus ADC_ACPConfig(ADC_ACP_TypeDef *ADC_ACP_InitStruct)
{
    if(ADC_ACP_InitStruct->ACPC_OVER_TIME > ADC_ACPC_OVFL_TIME_MAX
       ||ADC_ACP_InitStruct->ACPC_TIMES > ADC_ACPC_TIMES_MAX
       ||ADC_ACP_InitStruct->ACPC_MIN_TARGET > ADC_ACP_MIN_MAX
       ||ADC_ACP_InitStruct->ACPC_MAX_TARGET > ADC_ACP_MAX_MAX)
    {
        return ERROR;
    }

    if(ADC_ACP_InitStruct->ACP_EN == ENABLE)
    {
        if(ADC->CON1.CLKS == ADC_ClkS_PCLK)
        {
            ADC->ACPC.CLKS = ADC_ACPC_CLKS_PCLK;
        }
        else
        {
            ADC->ACPC.CLKS = ADC_ACPC_CLKS_LRC;
        }

        ADC->ACPC.OVFL_TIME = ADC_ACP_InitStruct ->ACPC_OVER_TIME;
        ADC->ACPC.TIMES = ADC_ACP_InitStruct->ACPC_TIMES;
        ADC->IE.ACPOVIE = ENABLE;
       
        if(ADC_ACP_InitStruct ->ACPC_MAX_TARGET ==0x0)
        {
            ADC->IE.ACPMAXIE = DISABLE;
        }
        else
        {
            ADC->ACPCMP.CMP_MAX =ADC_ACP_InitStruct ->ACPC_MAX_TARGET;
            ADC->IE.ACPMAXIE = ENABLE;
        }
        
        if(ADC_ACP_InitStruct ->ACPC_MIN_TARGET == 0xfff)
        {
            ADC->IE.ACPMINIE = DISABLE;
        }
        else
        {
            ADC->ACPCMP.CMP_MIN =ADC_ACP_InitStruct ->ACPC_MIN_TARGET;
            ADC->IE.ACPMINIE = ENABLE;
        }

        ADC->CON0.ACP_EN = ENABLE;

    }
    else
    {
        ADC->CON0.ACP_EN = DISABLE;
    }

    return SUCCESS;
}

ErrorStatus ADC_SoftStart(void)
{
    if(ADC->CON1.SMPS == ADC_SMPS_HARD)
        return ERROR;

    ADC ->CON1.SMPON = SET;
    return SUCCESS;
}

ErrorStatus ADC_SoftStop(void)
{
    if(ADC->CON1.SMPS == ADC_SMPS_HARD)
        return ERROR;

    ADC->CON1.SMPON = RESET;
    return SUCCESS;
}

uint16_t ADC_GetACPMeanValue(void)
{
    return ((uint16_t)ADC->ACPMEAN.MEAN_DATA);
}

uint16_t ADC_GetACPMinValue(void)
{
    return ((uint16_t)ADC->ACPCMP.CMP_MIN);
}

uint16_t ADC_GetACPMaxValue(void)
{
    return ((uint16_t)ADC->ACPCMP.CMP_MAX);
}

FlagStatus ADC_GetFlagStatus(ADC_TYPE_IF IFName)
{
    FlagStatus bitstatus = RESET;

    if (((ADC->IF.Word & IFName) != (uint32_t)RESET))
        bitstatus = SET;                    
    else
        bitstatus = RESET;                  

    return  bitstatus;
}

ITStatus ADC_GetITStatus(ADC_TYPE_IE IEName)
{
    ITStatus bitstatus = RESET;

    if (((ADC->IE.Word & IEName) != (uint32_t)RESET))
        bitstatus = SET;                    
    else
        bitstatus = RESET;                 

    return  bitstatus;
}

ErrorStatus ADC_ClearIFStatus(ADC_TYPE_IF IFName)
{
    ADC->IF.Word = (uint32_t)IFName;

    return SUCCESS;
}

void ADC_Reset(void)
{
    ADC->CON0.Word = 0x00000030;
    ADC->CON1.Word = 0x00041000;
    ADC->CHS.Word = 0x00000100;
    ADC->IE.Word = 0x00000000;
    ADC->IF.Word = 0x00000000;
    ADC->ACPC.Word = 0x00000000;
    ADC->ACPCMP.Word = 0x0FFF0000;
    ADC->VREFCON.Word = 0x00000000;
}

/*************************END OF FILE**********************/


