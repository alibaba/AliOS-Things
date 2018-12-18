/***************************************************************
 *Copyright (C), 2015, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/

#ifndef __LIBADC_H__
#define __LIBADC_H__

#include "HR8P2xx.h"

#include "type.h"

typedef enum
{
    ADC_ClkS_Pclk  = 0x0,
    ADC_ClkS_32Khz = 0x1,
} ADC_TYPE_CLKS;

typedef enum
{
    ADC_ClkDiv_1   = 0x0,
    ADC_ClkDiv_2   = 0x1,
    ADC_ClkDiv_4   = 0x2,
    ADC_ClkDiv_8   = 0x3,
    ADC_ClkDiv_16  = 0x4,
    ADC_ClkDiv_32  = 0x5,
    ADC_ClkDiv_64  = 0x6,
    ADC_ClkDiv_256 = 0x7,
} ADC_TYPE_CLKDIV;

typedef enum
{
    ADC_Align_Right = 0x1,
    ADC_Align_Left  = 0x0,
} ADC_TYPE_FM;

typedef enum
{
    ADC_VrefP_Vcc   = 0x0,
    ADC_VrefP_Ref   = 0x1,
    ADC_VrefP_Ref2  = 0x2,
    ADC_VrefP_Exter = 0x3,
} ADC_TYPE_VREFP;

typedef enum
{
    ADC_Samp_Soft     = 0x0,
    ADC_Samp_Hardware = 0x1,
} ADC_TYPE_SMPS;

typedef enum
{
    ADC_SampClk_2  = 0x0,
    ADC_SampClk_4  = 0x1,
    ADC_SampClk_8  = 0x2,
    ADC_SampClk_16 = 0x3,
} ADC_TYPE_ST;

typedef enum
{
    ADC_ConvSpeed_Low  = 0x0,
    ADC_ConvSpeed_High = 0x1,
} ADC_TYPE_HSEN;

typedef enum
{
    ADC_Ch_0  = 0x0,
    ADC_Ch_1  = 0x1,
    ADC_Ch_2  = 0x2,
    ADC_Ch_3  = 0x3,
    ADC_Ch_4  = 0x4,
    ADC_Ch_5  = 0x5,
    ADC_Ch_6  = 0x6,
    ADC_Ch_7  = 0x7,
    ADC_Ch_8  = 0x8,
    ADC_Ch_9  = 0x9,
    ADC_Ch_10 = 0xa,
} ADC_TYPE_CHS;

typedef enum
{
    ADC_LP_86ua  = 2,
    ADC_LP_215ua = 0,
    ADC_LP_120ua = 1,
    ADC_LP_70ua  = 3,
} ADC_TYPE_BUFLP;

typedef enum
{
    ADC_BUFF_0ua  = 0,
    ADC_BUFF_34ua = 1,
    ADC_BUFF_95ua = 2,
} ADC_TYPE_BUFEN;

typedef struct
{
    ADC_TYPE_CLKS  ADC_ClkS;

    ADC_TYPE_CLKDIV  ADC_ClkDiv;

    ADC_TYPE_FM  ADC_Align;

    ADC_TYPE_VREFP  ADC_VrefP;

    ADC_TYPE_SMPS  ADC_SampS;

    ADC_TYPE_ST  ADC_SampClk;

    ADC_TYPE_HSEN  ADC_ConvSpeed;

    ADC_TYPE_CHS  ADC_ChS;

    ADC_TYPE_BUFLP  ADC_Lp;

    ADC_TYPE_BUFEN ADC_Buf;

} ADC_InitStruType;

#define ADC_Enable()  (ADC->CON0.EN = 0x1)
#define ADC_Disable() (ADC->CON0.EN = 0x0)

#define ADC_Start() (ADC->CON0.TRG = 0x1)

#define ADC_SampStart() (ADC->CON1.SMPON = 0x1)
#define ADC_SampStop()  (ADC->CON1.SMPON = 0x0)

#define ADC_ClearITPendingBit() (ADC->IF.IF = 0x1)

#define ADC_IT_Enable()  (ADC->IE.IE = 0x1)
#define ADC_IT_Disable() (ADC->IE.IE = 0x0)

void ADC_Init(ADC_InitStruType * ADC_InitStruct);
void ADC_Set_CH(ADC_TYPE_CHS AdcCH);
uint16_t ADC_GetConvValue(void);
FlagStatus ADC_GetConvStatus(void);
FlagStatus ADC_GetFlagStatus(void);
ITStatus ADC_GetITStatus(void);
void ADC_Reset(void);
#endif

/*************************END OF FILE**********************/
