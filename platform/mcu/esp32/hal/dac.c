/*
 * Copyright (C) 2015-2018 Alibaba Group Holding Limited
 */

#include <stdio.h>
#include <stdint.h>
#include <aos/aos.h>
#include <hal/soc/dac.h>
#include "driver/dac.h"
#include "soc/soc.h"

/*DAC channel 1 is attached to GPIO25, DAC channel 2 is attached to GPIO26*/

typedef struct esp32_dac{
	uint32_t channel1_value;
	uint32_t channel2_value;
}esp32_dac_t;

static esp32_dac_t g_dac = {0x00,0x00};

int32_t hal_dac_init(dac_dev_t *dac){
	return (0);
}

int32_t hal_dac_start(dac_dev_t *dac, uint32_t channel){
	if(DAC_CHANNEL_1 != channel && DAC_CHANNEL_2 != channel){
		return(-1);
	}
	return dac_output_enable(channel);
}

int32_t hal_dac_stop(dac_dev_t *dac, uint32_t channel){
	if(DAC_CHANNEL_1 != channel && DAC_CHANNEL_2 != channel){
		return(-1);
	}
	return dac_output_disable(channel);
	
}

int32_t hal_dac_set_value(dac_dev_t *dac, uint32_t channel, uint32_t data){
	
	if(DAC_CHANNEL_1 == channel){
		g_dac.channel1_value = data; 
	}else if(DAC_CHANNEL_2 == channel){
		g_dac.channel2_value = data;
	}else{
		return(-1);
	}
	return dac_output_voltage(channel,data);	
	
}

int32_t hal_dac_get_value(dac_dev_t *dac, uint32_t channel){
	
	if(DAC_CHANNEL_1 == channel){
		return (g_dac.channel1_value);
	}else if(DAC_CHANNEL_2 == channel){
		return (g_dac.channel2_value);
	}
	return(-1);
}

int32_t hal_dac_finalize(dac_dev_t *dac){
	return (0);
}