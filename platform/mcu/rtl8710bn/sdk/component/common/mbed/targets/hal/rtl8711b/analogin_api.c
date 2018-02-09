/** mbed Microcontroller Library
  ******************************************************************************
  * @file    analogin_api.c
  * @author 
  * @version V1.0.0
  * @date    2016-08-01
  * @brief   This file provides mbed API for ADC.
  ******************************************************************************
  * @attention
  *
  * This module is a confidential and proprietary property of RealTek and
  * possession or use of this module requires written permission of RealTek.
  *
  * Copyright(c) 2016, Realtek Semiconductor Corporation. All rights reserved.
  ****************************************************************************** 
  */

#include "objects.h"
#include "PinNames.h"
#include "analogin_api.h"

#include "pinmap.h"

static ADC_InitTypeDef AdcInitStruct; 

extern u32 ConfigDebugErr;
extern u32 ConfigDebuginfo;

typedef enum {
	ADC_CH0	=	0,
	ADC_CH1	=	1,
	ADC_CH2	=	2,
	ADC_CH3	=	3,
} ADC_CH;

static const PinMap PinMap_ADC[] = {
	{PA_19,			ADC_CH1,		0},
	{VBAT_MEAS,		ADC_CH2,		0},
	{PA_20,			ADC_CH3,		0},
	{NC,			NC,				0}
};

/**
  * @brief  Read 8 bytes data from ADC.
  * @param  pBuf: 8 bytes buffer for data read.
  * @note poll mode will be used in this function.
  * @retval None
  */
void ADC_ReceiveBuf_ACUT(u32 *pBuf)
{
	u32 isr = ADC_GetISR();
	ADC_TypeDef *adc = ADC;
	u32	AdcTempDat;
	
	/* Clear ADC Status */
	ADC_INTClear();
	ADC_INTConfig(BIT_ADC_FIFO_FULL_EN|BIT_ADC_FIFO_RD_REQ_EN, ENABLE);

	ADC_Cmd(ENABLE);

	/* B CUT ADD patch for reset fail */
	AdcTempDat = adc->ANAPAR_AD1;
	AdcTempDat |= BIT(0);
	adc->ANAPAR_AD1 = AdcTempDat;

	while (1) {
		isr = ADC_GetISR();
		if (isr & (BIT_ADC_FIFO_FULL | BIT_ADC_FIFO_RD_REQ)) {
			*pBuf = (u32)ADC_Read();
			*(pBuf+1)= (u32)ADC_Read();

			ADC_INTClear();
			ADC_INTConfig(BIT_ADC_FIFO_FULL_EN|BIT_ADC_FIFO_RD_REQ_EN, DISABLE);
			
			break;
		}
	}

	/* B CUT ADD patch for reset fail */
	AdcTempDat = adc->ANAPAR_AD1;
	AdcTempDat &= ~BIT(0);
	adc->ANAPAR_AD1 = AdcTempDat;

	ADC_Cmd(DISABLE);

	/* Clear ADC Status */
	ADC_INTClear();
}

/**
  * @brief  Initializes the ADC device, include clock/function/ADC registers.
  * @param  obj: adc object define in application software.
  * @param  pin: adc PinName according to pinmux spec.
  * @retval none  
  */
void analogin_init(analogin_t *obj, PinName pin)
{
	uint32_t adc_idx;

	ConfigDebugErr &= (~(_DBG_ADC_|_DBG_GDMA_));
	ConfigDebugInfo&= (~(_DBG_ADC_|_DBG_GDMA_));

	adc_idx = pinmap_peripheral(pin, PinMap_ADC);
	DBG_8195A("analogin_init [%x:%x ]\n", pin, adc_idx);
	assert_param(adc_idx != NC);

	/* Set ADC Device Number */
	obj->adc_idx = adc_idx;

	/* Load ADC default value */
	ADC_InitStruct(&AdcInitStruct);
	ADC_AnaparAd[1] = 0x81004;
	
	/* Init ADC now */
	/* ADC Interrupt disable, poll mode will be used */
	InterruptDis(ADC_IRQ);

	/* To release ADC delta sigma clock gating */
	PLL2_Set(BIT_SYS_SYSPLL_CK_ADC_EN, ENABLE);

	/* Turn on ADC active clock */
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, ENABLE);
	
	ADC_Init(&AdcInitStruct);
}

/**
  * @brief  Reads data from the specified adc channel fifo.
  * @param  obj: adc object define in application software.
  * @retval adc channel data(float) 
  */
float analogin_read(analogin_t *obj)
{
	float value;
	uint32_t AnaloginTmp[2] = {0,0};
	uint32_t AnaloginDatMsk = 0xFFFF;
	uint8_t  AnaloginIdx = obj->adc_idx;
	uint32_t AnalogDat = 0;
	uint32_t AnalogDatFull = 0;

	ADC_ReceiveBuf_ACUT(&AnaloginTmp[0]);

	AnaloginDatMsk = (u32)(AnaloginDatMsk<<((u32)(16*(AnaloginIdx&0x01))));
	AnalogDat = AnaloginTmp[(AnaloginIdx/2)];
	AnalogDat = (AnalogDat & AnaloginDatMsk);    
	AnalogDat = (AnalogDat>>((u32)(16*(AnaloginIdx&0x01))));

	AnalogDatFull   = 0xCE80;

	value = (float)(AnalogDat) / (float)(AnalogDatFull);

	return (float)value;
}

/**
  * @brief  Reads data from the specified adc channel fifo.
  * @param  obj: adc object define in application software.
  * @retval 16bit adc channel data(int)
  */
uint16_t analogin_read_u16(analogin_t *obj)
{
	uint32_t AnaloginTmp[2] = {0,0};
	uint32_t AnaloginDatMsk = 0xFFFF;
	uint8_t  AnaloginIdx = obj->adc_idx;
	uint32_t AnalogDat = 0;

	ADC_ReceiveBuf_ACUT(&AnaloginTmp[0]);

	//DBG_8195A("[0]:%08x, %08x\n", AnaloginTmp[0], AnaloginTmp[1]  );
	AnaloginDatMsk = (u32)(AnaloginDatMsk<<((u32)(16*(AnaloginIdx&0x01))));
	AnalogDat = AnaloginTmp[(AnaloginIdx/2)];
	AnalogDat = (AnalogDat & AnaloginDatMsk);    
	AnalogDat = (AnalogDat>>((u32)(16*(AnaloginIdx&0x01))));

	return (uint16_t)AnalogDat;
}

/**
  * @brief  Deinitializes the ADC device, include clock/function/ADC registers.
  * @param  obj: adc object define in application software.
  * @retval none  
  */
void  analogin_deinit(analogin_t *obj)
{
	/* Clear ADC Status */
	ADC_INTClear();
	
	/* Disable ADC  */
	ADC_Cmd(DISABLE);
	
	/* To release ADC delta sigma clock gating */
	PLL2_Set(BIT_SYS_SYSPLL_CK_ADC_EN, DISABLE);

	/* Turn on ADC active clock */
	RCC_PeriphClockCmd(APBPeriph_ADC, APBPeriph_ADC_CLOCK, DISABLE);
}
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
