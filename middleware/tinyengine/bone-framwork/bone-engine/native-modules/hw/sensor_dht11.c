/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */
#define CONFIG_LOGMACRO_DETAILS
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <be_osal.h>
#include "gpio.h"
#include "utils/be_common.h"
#include "be_jse_module.h"
#include "board-mgr/board_mgr.h"


static void  dht11_soft_delay(uint32_t counts){
		uint32_t i =0;
		for(i=0;i<counts;++i)(void)0;
}


static int8_t dht11_wait_status(gpio_dev_t * gpio_device,uint32_t maxCounts,int8_t level){

	uint32_t try_counts = 0;
	uint32_t value = 0;
	do{
		hal_gpio_input_get(gpio_device, &value);
		try_counts = try_counts + 1;
		dht11_soft_delay(200);
	}while((level == value)&& (try_counts < maxCounts));
	
	return (try_counts  >= maxCounts)?(-1):(0);
}


static int8_t dht11_read_bit(gpio_dev_t * gpio_device){

	int ret = -1;
	uint32_t value = 0;
	if(0 != dht11_wait_status(gpio_device,100,0)){
		return(-1);
	}
	dht11_soft_delay(200);
	hal_gpio_input_get(gpio_device, &value);
	if(1==value){
		if(0 != dht11_wait_status(gpio_device,100,1)){
			return(-1);
		}
		return 1;
	}
	return 0;
}


static int8_t  dht11_read_byte(gpio_dev_t * gpio_device,uint8_t raw_buff[5]){

	int8_t ret = -1;
	int32_t i = 0;
	int32_t j = 0;
	int8_t bit_value = 0;
	uint32_t value = 0;
	
	hal_gpio_output_low(gpio_device);
	be_osal_delay(20);
	hal_gpio_output_high(gpio_device);
	dht11_soft_delay(100);
	hal_gpio_input_get(gpio_device, &value);
	dht11_soft_delay(100);
	hal_gpio_input_get(gpio_device, &value);
	if(0 != value){
		be_error("dht11","dht11 wrong status!\n");
		return(-1);
	}
	if(0 != dht11_wait_status(gpio_device,1000,0)){

		be_error("dht11","dht11_wait_status fail!\n");
		return(-1);
	}
	if(0 != dht11_wait_status(gpio_device,1000,1)){
		be_error("dht11","dht11_wait_status fail!\n");
		return(-1);
	}
	for(i=0;i<5;++i){
		for(j=0;j<8;++j){
			raw_buff[i] <<= 1;
			bit_value = dht11_read_bit(gpio_device);
			if(-1 == bit_value){
				return(-1);
			}
			raw_buff[i] |= bit_value;
		}
	}
	if(0 != dht11_wait_status(gpio_device,1000,0)){
		return(-1);
	}
	return 0;	
}


be_jse_symbol_t * dht11_get_temphum(void){

	int8_t ret = -1;
	uint32_t i = 0;
	int8_t result = -1;
	uint8_t dht11_buff[5] = {0x00};
	uint8_t temp_hum[2] = {0x00,0x00};
	be_jse_symbol_t *arr = new_symbol(BE_SYM_ARRAY);
	item_handle_t gpio_handle;
	be_jse_symbol_t * arg0 = NULL;
	gpio_dev_t * gpio_device = NULL;
	be_jse_handle_function(0,&arg0,NULL,NULL,NULL);
	if(!arg0 || !symbol_is_int(arg0)){
		goto out;
	}
	gpio_handle.handle = get_symbol_value_int(arg0);
	gpio_device = board_get_node_by_handle(MODULE_GPIO,&gpio_handle);
	if(NULL == gpio_device){
		be_error("dht11","board_get_node_by_handle fail!\n");
		goto out;
	}
	
	if(0 != dht11_read_byte(gpio_device,dht11_buff)){
		be_error("dht11","dht11_read_byte fail!\n");
		goto out;
	}

	if(dht11_buff[0]+dht11_buff[1]+dht11_buff[2]+dht11_buff[3] == dht11_buff[4]){
		temp_hum[0] = dht11_buff[0];
		temp_hum[1] = dht11_buff[2];
	}
	result = 0;
out:
	symbol_unlock(arg0);
	for (i=0; i<2;++i) {
		be_jse_symbol_t *idx = new_int_symbol(temp_hum[i]);
		symbol_array_push(arr, idx);
		symbol_unlock(idx);
	}
	return  arr;
}

