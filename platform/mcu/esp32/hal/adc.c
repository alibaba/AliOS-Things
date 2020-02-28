/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include "aos/kernel.h"
#include "aos/hal/adc.h"
#include "driver/adc.h"
#include "soc/soc.h"

typedef struct adc_obj{
	uint32_t port;
	uint32_t channel;
}adc_obj_t;


static  adc_obj_t obj[]={

	{GPIO_NUM_36, ADC1_CHANNEL_0},
	{GPIO_NUM_37, ADC1_CHANNEL_1},
	{GPIO_NUM_38, ADC1_CHANNEL_2},
	{GPIO_NUM_39, ADC1_CHANNEL_3},
	{GPIO_NUM_32, ADC1_CHANNEL_4},
	{GPIO_NUM_33, ADC1_CHANNEL_5},
	{GPIO_NUM_34, ADC1_CHANNEL_6},
	{GPIO_NUM_35, ADC1_CHANNEL_7},
};


static int adc_get_channel(uint32_t port){
	
	int32_t i = 0;
	int32_t size = sizeof(obj)/sizeof(obj[0]);
	for(i=0;i<size;++i){
		if(port == obj[i].port)return obj[i].channel;
	}
	return -1;
}


int32_t hal_adc_init(adc_dev_t *adc){

	int32_t channel = adc_get_channel(adc->port);
	if(-1 == channel)return(-1);
	adc1_config_width(ADC_WIDTH_12Bit);
	adc1_config_channel_atten(channel, ADC_ATTEN_0db);
	return(0);
}


int32_t hal_adc_value_get(adc_dev_t *adc, uint32_t *output, uint32_t timeout){

	int32_t channel = adc_get_channel(adc->port);
	if(-1 == channel){
		*(int32_t*)(output) = -1;
		return(-1);
	}
	*(int32_t*)(output) = adc1_get_raw((adc1_channel_t)channel);
	return (0);
}


int32_t hal_adc_finalize(adc_dev_t *adc){

	return(0);

}

