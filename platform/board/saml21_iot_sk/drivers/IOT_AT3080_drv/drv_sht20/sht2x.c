/**
 ******************************************************************************
 * @file    sht2x.c
 * @author  William Xu
 * @version V1.0.0
 * @date    9-Apr-2018
 * @brief   SHT2x sensor driver
 ******************************************************************************
 *
 * Copyright (c) 2009-2018 MXCHIP Co.,Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 ******************************************************************************
 */


#include <stdio.h>

#include "mx_hal.h"
#include "sht2x.h"


/******************************************************************************
 *                                 Constants
 ******************************************************************************/

#define  I2C_ADDR_W 0x40
#define  I2C_ADDR_R 0x40

/******************************************************************************
 *                               Type Definitions
 ******************************************************************************/

typedef enum {
    TRIG_TEMP_MEASUREMENT_HM   = 0xE3,
    TRIG_HUMI_MEASUREMENT_HM   = 0xE5,
    TRIG_TEMP_MEASUREMENT_POLL = 0xF3,
    TRIG_HUMI_MEASUREMENT_POLL = 0xF5,
    USER_REG_W                 = 0xE6,
    USER_REG_R                 = 0xE7,
    SOFT_RESET                 = 0xFE
} SHT2xCommand;

typedef enum {
    SHT2x_RES_12_14BIT         = 0x00,
    SHT2x_RES_8_12BIT          = 0x01,
    SHT2x_RES_10_13BIT         = 0x80,
    SHT2x_RES_11_11BIT         = 0x81,
    SHT2x_RES_MASK             = 0x81
} SHT2xResolution;

typedef enum {
    SHT2x_HEATER_ON            = 0x04,
    SHT2x_HEATER_OFF           = 0x00,
    SHT2x_HEATER_MASK          = 0x04
} SHT2xHeater;

typedef struct{
    float TEMP_POLL;
    float HUMI_POLL;
} SHT2x_data;

/******************************************************************************
 *                              Variable Definitions
 ******************************************************************************/

static void *i2c = NULL;

/******************************************************************************
 *                              Function Definitions
 ******************************************************************************/

int ht20_i2c_bus_write(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int ret = 0;

	ret = mx_hal_i2c_cmd_write(i2c, I2C_ADDR_W, reg_addr, reg_data, cnt);
	require_noerr(ret, exit);

exit:
	return ret;
}


int ht20_i2c_bus_read(uint8_t reg_addr, uint8_t *reg_data, uint8_t cnt)
{
	int ret = 0;

	ret = mx_hal_i2c_cmd_read(i2c, I2C_ADDR_R, reg_addr, reg_data, cnt);
	require_noerr(ret, exit);

exit:	
	return ret;
}


uint8_t SHT2x_Init(void)
{
	uint8_t err = kNoErr;
   	i2c = mx_hal_i2c_init(NULL);
	   
	err = SHT2x_SoftReset();
	mx_hal_delay_ms(100);

	return err;
}

uint8_t SHT2x_SoftReset(void) //SHT20 Soft Reset
{
	return ht20_i2c_bus_write(SOFT_RESET, NULL, 0);
}


float SHT2x_GetTempPoll(void)
{
	float TEMP;
	uint8_t tmp[2];
	uint16_t ST;
	
	ht20_i2c_bus_read(TRIG_TEMP_MEASUREMENT_POLL, tmp, 2);
	ST = (tmp[0] << 8) | (tmp[1] << 0);

    ST &= ~0x0003;
    TEMP = ((float)ST * 0.00268127) - 46.85;

    return (TEMP);	  

}

float SHT2x_GetHumiPoll(void)
{
	float HUMI;
	uint8_t tmp[2];
	uint16_t SRH;

	ht20_i2c_bus_read(TRIG_HUMI_MEASUREMENT_POLL, tmp, 2);
	SRH = (tmp[0] << 8) | (tmp[1] << 0);

    SRH &= ~0x0003;
    HUMI = ((float)SRH * 0.00190735) - 6;

    return (HUMI);
}

