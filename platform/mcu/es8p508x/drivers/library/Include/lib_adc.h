/***************************************************************
 *Copyright (C), 2017, Shanghai Eastsoft Microelectronics Co., Ltd
 ***************************************************************/
#ifndef __LIBADC_H__
#define __LIBADC_H__

#include "ES8P508x.h"
#include "type.h"


typedef enum
{
    ADC_ClkS_PCLK = 0x0 ,   
    ADC_ClkS_PLL = 0x1 ,    
    ADC_ClkS_32K = 0x2 ,   
}ADC_TYPE_CLKS;

typedef enum
{
    ADC_ClkDiv_1 = 0x0 ,   
    ADC_ClkDiv_2 = 0x1 ,   
    ADC_ClkDiv_4 = 0x2 ,   
    ADC_ClkDiv_8 = 0x3 ,   
    ADC_ClkDiv_16 = 0x4 ,  
    ADC_ClkDiv_32 = 0x5 ,  
    ADC_ClkDiv_64 = 0x6 ,  
    ADC_ClkDiv_256 = 0x7 , 
}ADC_TYPE_CLKDIV;


typedef enum
{
    ADC_VrefP_Vcc = 0x0 ,    
    ADC_VrefP_Ref = 0x1 ,    
    ADC_VrefP_Ref2 = 0x2 ,    
    ADC_VrefP_Exter = 0x3 ,  
}ADC_TYPE_VREFP;

typedef enum
{
    ADC_SMPS_SOFT = 0,      
    ADC_SMPS_HARD = 1, 
}ADC_TYPE_SMPS;

typedef enum
{
    ADC_SampClk_2 = 0x0 ,  
    ADC_SampClk_4 = 0x1 ,  
    ADC_SampClk_8 = 0x2 ,  
    ADC_SampClk_16 = 0x3 , 
}ADC_TYPE_ST;

typedef enum
{
    ADC_ConvSpeed_Low = 0x0 ,  
    ADC_ConvSpeed_High = 0x1 , 
}ADC_TYPE_HSEN;

typedef enum
{
    ADC_CHS_AIN0 = 0,       
    ADC_CHS_AIN1 = 1,       
    ADC_CHS_AIN2 = 2,       
    ADC_CHS_AIN3 = 3,       
    ADC_CHS_AIN4 = 4,      
    ADC_CHS_AIN5 = 5,      
    ADC_CHS_AIN6 = 6,      
    ADC_CHS_AIN7 = 7,      
    ADC_CHS_AIN8 = 8,      
    ADC_CHS_AIN9 = 9,      
    ADC_CHS_AIN10 = 10,    
    ADC_CHS_AIN11 = 11,    
    ADC_CHS_AIN12 = 12,    
    ADC_CHS_AIN13 = 13,    
    ADC_CHS_AIN14 = 14,    
    ADC_CHS_AIN15 = 15,     
}ADC_TYPE_CHS;

typedef enum
{
    ADC_LP_86ua = 2,     
    ADC_LP_215ua = 0,    
    ADC_LP_120ua = 1,    
    ADC_LP_70ua = 3,    
}ADC_TYPE_BUFLP;

typedef enum
{
    ADC_BUFF_0ua = 0,     
    ADC_BUFF_34ua = 1,    
    ADC_BUFF_95ua = 2,     
}ADC_TYPE_BUFEN;

typedef enum 
{
    ADC_IF = 0x01,
    ADC_IF_ACPMIN = 0x02,
    ADC_IF_ACPMAX =0x04,
    ADC_IF_ACPOVER = 0x08,
}ADC_TYPE_IF;

typedef enum 
{
    ADC_IE = 0x01,
    ADC_IE_ACPMIN = 0x02,
    ADC_IE_ACPMAX =0x04,
    ADC_IE_ACPOVER = 0x08,
}ADC_TYPE_IE;

typedef struct
{
    ADC_TYPE_CLKS  ADC_ClkS;      
    ADC_TYPE_CLKDIV  ADC_ClkDiv;  
    ADC_TYPE_VREFP  ADC_VrefP;    
    ADC_TYPE_SMPS  ADC_SampS;     
    ADC_TYPE_ST  ADC_SampClk;     
    ADC_TYPE_HSEN  ADC_ConvSpeed; 
    ADC_TYPE_CHS  ADC_ChS;        
    TYPE_FUNCEN ADC_IREF_EN;
    TYPE_FUNCEN ADC_VREF_EN;
    TYPE_FUNCEN ADC_VCMBUF_EN;
    TYPE_FUNCEN ADC_VREFN;
    TYPE_FUNCEN ADC_VRBUF_EN;
}ADC_InitStruType;

typedef enum
{
    ADC_ACP_MIN_MIN = 0x0,      
    ADC_ACP_MIN_MAX = 0xfff,    
}ADC_TYPE_ACP_MIN;

typedef enum
{
    ADC_ACP_MAX_MIN = 0x0,  
    ADC_ACP_MAX_MAX = 0xfff,    
}ADC_TYPE_ACP_MAX;

typedef enum
{
    ADC_ACPC_OVFL_TIME_MAX = 0x9c3, 
}ADC_TYPE_ACPC_OVFL_TIME;

typedef enum
{
    ADC_ACPC_TIMES_1 = 0,   
    ADC_ACPC_TIMES_2 = 1,   
    ADC_ACPC_TIMES_4 = 2,   
    ADC_ACPC_TIMES_8 = 3,   
    ADC_ACPC_TIMES_MAX = 3,
}ADC_TYPE_ACPC_TIMES;

typedef enum
{
    ADC_ACPC_CLKS_PCLK = 0,              
    ADC_ACPC_CLKS_LRC = 1,               
}ADC_TYPE_ACPC_CLKS;

typedef FuncState ADC_TYPE_CHOP_EN1, ADC_TYPE_CHOP_EN;
typedef FuncState ADC_TYPE_VREF_EN,ADC_TYPE_IREF_EN;
typedef FuncState ADC_TYPE_EN,ADC_TYPE_ACP_EN;

typedef struct
{
    ADC_TYPE_ACP_EN ACP_EN;                 
    uint16_t ACPC_OVER_TIME;                
    ADC_TYPE_ACPC_TIMES ACPC_TIMES;         
    uint16_t ACPC_MIN_TARGET;              
    uint16_t ACPC_MAX_TARGET;              
}ADC_ACP_TypeDef;


#define ADC_Enable()  (ADC->CON0.EN = 0x1)
#define ADC_Disable() (ADC->CON0.EN = 0x0)

#define ADC_ACP_Enable() (ADC->CON0.ACP_EN = 1)
#define ADC_ACP_Disable() (ADC->CON0.ACP_EN = 0)

#define ADC_Start() (ADC->CON0.TRIG = 0x1)

#define ADC_SampStart() (ADC->CON1.SMPON = 0x1)
#define ADC_SampStop()  (ADC->CON1.SMPON = 0x0)


#define ADC_VDD5_FLAG_Enable() (ADC->CHS.VDD5_FLAG_EN = 0x1)
#define ADC_VDD5_FLAG_Disable()  (ADC->CHS.VDD5_FLAG_EN = 0x0)


#define ADC_IE_Enable()  (ADC->IE.IE = 0x1)
#define ADC_IE_Disable() (ADC->IE.IE = 0x0)

#define ADC_ACPMINIE_Enable() (ADC->IE.ACPMINIE = 1)
#define ADC_ACPMINIE_Disable() (ADC->IE.ACPMINIE = 0)

#define ADC_ACPMAXIE_Enable() (ADC->IE.ACPMAXIE = 1)
#define ADC_ACPMAXIE_Disable() (ADC->IE.ACPMAXIE = 0)

#define ADC_ACPOVIE_Enable() (ADC->IE.ACPOVIE = 1)
#define ADC_ACPOVIE_Disable() (ADC->IE.ACPOVIE = 0)


void ADC_Init(ADC_InitStruType * ADC_InitStruct);
void ADC_Set_CH(ADC_TYPE_CHS AdcCH);
uint16_t ADC_GetConvValue(void);
FlagStatus ADC_GetConvStatus(void);
ErrorStatus ADC_ACPConfig(ADC_ACP_TypeDef *ADC_ACP_InitStruct);
ErrorStatus ADC_SoftStart(void);
ErrorStatus ADC_SoftStop(void);
uint16_t ADC_GetACPMeanValue(void);
uint16_t ADC_GetACPMinValue(void);
uint16_t ADC_GetACPMaxValue(void);
FlagStatus ADC_GetFlagStatus(ADC_TYPE_IF IFName);
ITStatus ADC_GetITStatus(ADC_TYPE_IE IEName);
ErrorStatus ADC_ClearIFStatus(ADC_TYPE_IF IFName);
void ADC_Reset(void);

#endif

/*************************END OF FILE**********************/
