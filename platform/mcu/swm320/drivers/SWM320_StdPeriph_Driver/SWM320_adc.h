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

#ifndef __SWM320_ADC_H__
#define __SWM320_ADC_H__

typedef struct {
    uint8_t clk_src;
    uint8_t clk_div;
    uint8_t pga_ref;
    uint8_t channels;
    uint8_t samplAvg;
    uint8_t trig_src;
    uint8_t Continue;
    uint8_t EOC_IEn;
    uint8_t OVF_IEn;
    uint8_t HFULL_IEn;
    uint8_t FULL_IEn;
} ADC_InitStructure;

#define ADC_CH0     0x01
#define ADC_CH1     0x02
#define ADC_CH2     0x04
#define ADC_CH3     0x08
#define ADC_CH4     0x10
#define ADC_CH5     0x20
#define ADC_CH6     0x40
#define ADC_CH7     0x80

#define ADC_CLKSRC_HRC          1
#define ADC_CLKSRC_VCO_DIV16    2
#define ADC_CLKSRC_VCO_DIV32    3
#define ADC_CLKSRC_VCO_DIV64    4

#define ADC_AVG_SAMPLE1         0
#define ADC_AVG_SAMPLE2         1
#define ADC_AVG_SAMPLE4         3
#define ADC_AVG_SAMPLE8         7
#define ADC_AVG_SAMPLE16        15

#define ADC_TRIGSRC_SW          0
#define ADC_TRIGSRC_PWM         1

#define PGA_REF_INTERNAL        1
#define PGA_REF_EXTERNAL        0


void ADC_Init(ADC_TypeDef * ADCx, ADC_InitStructure * initStruct);
void ADC_Open(ADC_TypeDef * ADCx);
void ADC_Close(ADC_TypeDef * ADCx);
void ADC_Start(ADC_TypeDef * ADCx);
void ADC_Stop(ADC_TypeDef * ADCx);

uint32_t ADC_Read(ADC_TypeDef * ADCx, uint32_t chn);
uint32_t ADC_IsEOC(ADC_TypeDef * ADCx, uint32_t chn);

void ADC_ChnSelect(ADC_TypeDef * ADCx, uint32_t chns);

void ADC_IntEOCEn(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntEOCDis(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntEOCClr(ADC_TypeDef * ADCx, uint32_t chn);
uint32_t ADC_IntEOCStat(ADC_TypeDef * ADCx, uint32_t chn);

void ADC_IntOVFEn(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntOVFDis(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntOVFClr(ADC_TypeDef * ADCx, uint32_t chn);
uint32_t ADC_IntOVFStat(ADC_TypeDef * ADCx, uint32_t chn);

void ADC_IntHFULLEn(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntHFULLDis(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntHFULLClr(ADC_TypeDef * ADCx, uint32_t chn);
uint32_t ADC_IntHFULLStat(ADC_TypeDef * ADCx, uint32_t chn);

void ADC_IntFULLEn(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntFULLDis(ADC_TypeDef * ADCx, uint32_t chn);
void ADC_IntFULLClr(ADC_TypeDef * ADCx, uint32_t chn);
uint32_t ADC_IntFULLStat(ADC_TypeDef * ADCx, uint32_t chn);


#endif //__SWM320_ADC_H__
