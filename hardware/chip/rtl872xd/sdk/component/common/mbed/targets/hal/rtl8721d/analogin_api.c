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

static const PinMap PinMap_ADC[] = {
	{PB_4,			ADC_CH0,		0},
	{PB_5,			ADC_CH1,		0},
	{PB_6,			ADC_CH2,		0},
	{PB_7,			ADC_CH3,		0},
	{PB_1,			ADC_CH4,		0},
	{PB_2,			ADC_CH5,		0},
	{PB_3,			ADC_CH6,		0},
	{VBAT_MEAS,		ADC_CH7,		0},
	{NC,			NC,				0}
};

/** @defgroup AmebaD_Mbed_API 
  * @{
  */

/** @defgroup analog_in MBED_ADC
 *  @brief      MBED_ADC driver modules.
 *  @{
 */

/** @defgroup MBED_ADC_Exported_Functions MBED_ADC Exported Functions
  * @{
  */

/**
  * @brief  Initializes the ADC device, include clock/function/ADC registers.
  * @param  obj: adc object define in application software.
  * @param  pin: adc PinName according to pinmux spec.
  * @retval none  
  */
void analogin_init(analogin_t *obj, PinName pin)
{
	ADC_InitTypeDef ADC_InitStruct; 
	uint32_t adc_idx;

	adc_idx = pinmap_peripheral(pin, PinMap_ADC);
	DBG_8195A("analogin_init [%x:%x ]\n", pin, adc_idx);
	assert_param(adc_idx != NC);

	/* Set ADC channel Number */
	obj->adc_idx = adc_idx;

	/* Initialize ADC */
	ADC_StructInit(&ADC_InitStruct);
	ADC_InitStruct.ADC_CvlistLen = 0;
	ADC_Init(&ADC_InitStruct);

	ADC_Cmd(ENABLE);
}

/**
  * @brief  Reads data from the specified adc channel fifo.
  * @param  obj: adc object define in application software.
  * @retval adc channel data(float) 
  */
float analogin_read(analogin_t *obj)
{
	float value;
	uint32_t AnalogDatFull = 0xFFF;
	uint8_t  ChIdx = obj->adc_idx;
	uint32_t data;

	/* Set channel index into channel switch list*/
	ADC->ADC_CHSW_LIST[0] = ChIdx;

	/* Clear FIFO */
	ADC_ClearFIFO();

	/* SW trigger to sample */
	ADC_SWTrigCmd(ENABLE);
	while(ADC_Readable()== 0);
	ADC_SWTrigCmd(DISABLE);

	data = ADC_Read();
	value = (float)(data) / (float)(AnalogDatFull);

	return (float)value;
}

/**
  * @brief  Reads data from the specified adc channel fifo.
  * @param  obj: adc object define in application software.
  * @retval 16bit adc channel data(int)
  */
uint16_t analogin_read_u16(analogin_t *obj)
{
	uint8_t  ChIdx = obj->adc_idx;
	uint32_t data;

	/* Set channel index into channel switch list*/
	ADC->ADC_CHSW_LIST[0] = ChIdx;

	/* Clear FIFO */
	ADC_ClearFIFO();

	/* SW trigger to sample */
	ADC_SWTrigCmd(ENABLE);
	while(ADC_Readable()== 0);
	ADC_SWTrigCmd(DISABLE);

	data = ADC_Read();

	return (uint16_t)(data & BIT_MASK_DAT_GLOBAL);
}

/**
  * @brief  Deinitializes the ADC device, include clock/function/ADC registers.
  * @param  obj: adc object define in application software.
  * @retval none  
  */
void  analogin_deinit(analogin_t *obj)
{
	/* To avoid gcc warnings */
	( void ) obj;
	
	/* Clear ADC Status */
	ADC_INTClear();
	
	/* Disable ADC  */
	ADC_Cmd(DISABLE);
}
/** 
  * @}
  */

/** 
  * @}
  */

/** 
  * @}
  */
/******************* (C) COPYRIGHT 2016 Realtek Semiconductor *****END OF FILE****/
