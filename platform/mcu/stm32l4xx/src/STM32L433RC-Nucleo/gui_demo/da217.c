#include "stm32l4xx_hal.h"
#include "soc_init.h"
#include <string.h>
#include <stdio.h>

#define DA217_I2C_ADDR 0x4e
#define DA217_WAIT_TIME 100
#define DA217_WAIT_INTERVAL 50

#ifdef ALIOS_HAL
static int gsreg_mask_write(uint8_t reg, uint8_t mask, uint8_t data)
{
	uint8_t tdata = 0;
	uint8_t *pd = &tdata;
	int ret = 0;

	ret = hal_i2c_mem_read(&brd_i2c1_dev, DA217_I2C_ADDR, (uint16_t)reg, 1, pd, 1, 1000);
	if (ret != 0)
		return ret;
	tdata &= ~mask;
	tdata |= data & mask;
	ret = hal_i2c_mem_write(&brd_i2c1_dev, DA217_I2C_ADDR, (uint16_t)reg, 1, pd, 1, 1000);
	return ret;
}

static int gsreg_write(uint8_t reg, uint8_t data)
{
	uint8_t tdata = data;
	uint8_t *pd = &tdata;

	return hal_i2c_mem_write(&brd_i2c1_dev, DA217_I2C_ADDR, (uint16_t)reg, 1, pd, 1, 1000);
}

static int gsreg_read(uint8_t reg, uint8_t *data)
{
	return  hal_i2c_mem_read(&brd_i2c1_dev, DA217_I2C_ADDR, (uint16_t)reg, 1, data, 1, 1000);
}

static int gsreg_multi_read(uint8_t reg, uint8_t *data, uint16_t count)
{
	return  hal_i2c_mem_read(&brd_i2c1_dev, DA217_I2C_ADDR, (uint16_t)reg, 1, data, count, 1000);
}
#else
extern I2C_HandleTypeDef hi2c1;

static int gsreg_mask_write(uint8_t reg, uint8_t mask, uint8_t data)
{
	uint8_t tdata = 0;
	uint8_t *pd = &tdata;
	int ret = 0;

	ret = (int)HAL_I2C_Mem_Read(&hi2c1, DA217_I2C_ADDR, (uint16_t)reg, 1, pd, 1, HAL_MAX_DELAY);
	if (ret != 0)
		return ret;
	tdata &= ~mask;
	tdata |= data & mask;
	ret = (int)HAL_I2C_Mem_Write(&hi2c1, DA217_I2C_ADDR, (uint16_t)reg, 1, pd, 1, HAL_MAX_DELAY);
	return ret;
}

static int gsreg_write(uint8_t reg, uint8_t data)
{
	uint8_t tdata = data;
	uint8_t *pd = &tdata;

	return (int)HAL_I2C_Mem_Write(&hi2c1, DA217_I2C_ADDR, (uint16_t)reg, 1, pd, 1, HAL_MAX_DELAY);
}

static int gsreg_read(uint8_t reg, uint8_t *data)
{
	return  (int)HAL_I2C_Mem_Read(&hi2c1, DA217_I2C_ADDR, (uint16_t)reg, 1, data, 1, HAL_MAX_DELAY);
}

static int gsreg_multi_read(uint8_t reg, uint8_t *data, uint16_t count)
{
	return  (int)HAL_I2C_Mem_Read(&hi2c1, DA217_I2C_ADDR, (uint16_t)reg, 1, data, count, HAL_MAX_DELAY);
}
#endif

static int mir3da_set_enable(int enable)
{
	int res = 0;

	if (enable)
		res = gsreg_write(NSA_REG_POWERMODE_BW, 0x30);
	else
		res = gsreg_write(NSA_REG_POWERMODE_BW, 0x80);

	return res;	
}

#if 0
static int mir3da_is_ready(void)
{
	uint8_t data = 0;
	int res = 0;

	res = gsreg_read(NSA_REG_NEWDATA_FLAG, &data);
	if (res != 0)
		return 0;
	res = (data & 0x1) ? 1 : 0;

	return res;
}
#endif

int mir3da_open_interrupt(int num)
{
	int res = 0;

	res = gsreg_write(NSA_REG_INTERRUPT_SETTINGS1, 0x07);
	res = gsreg_write(NSA_REG_ACTIVE_DURATION, 0x00);
	res = gsreg_write(NSA_REG_ACTIVE_THRESHOLD, 0x3f);

	switch(num) {
	case 1:
		res = gsreg_write(NSA_REG_INTERRUPT_MAPPING1, 0x04);
		break;
	case 2:
		res = gsreg_write(NSA_REG_INTERRUPT_MAPPING3, 0x04);
		break;
	default:
		break;
	}

	return res;
}

int mir3da_close_interrupt(int num)
{
	int res = 0;

	res = gsreg_write(NSA_REG_INTERRUPT_SETTINGS1, 0x00);

	switch(num) {
	case 1:
		res = gsreg_write(NSA_REG_INTERRUPT_MAPPING1, 0x00);
		break;
	case 2:
		res = gsreg_write(NSA_REG_INTERRUPT_MAPPING3, 0x00);
		break;
	default:
		break;
	}

	return res;
}

int mir3da_open_step_counter(void)
{
	int res = 0;

	res |= gsreg_write(NSA_REG_STEP_CONGIF1, 0x01);
	res |= gsreg_write(NSA_REG_STEP_CONGIF2, 0x62);
	res |= gsreg_write(NSA_REG_STEP_CONGIF3, 0x46);
	res |= gsreg_write(NSA_REG_STEP_CONGIF4, 0x32);
	res |= gsreg_write(NSA_REG_STEP_FILTER, 0xa2);	

	return res;
}

int mir3da_close_step_counter(void)
{
	return gsreg_write(NSA_REG_STEP_FILTER, 0x22);	
}

int mir3da_read_data(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t tmp_data[6] = {0};
//	int wait_count = DA217_WAIT_TIME;
//	int res = 0;

	if (gsreg_multi_read(NSA_REG_ACC_X_LSB, (uint8_t *)tmp_data, 6) != 0) {
		return -1;
	}

#if 0
	do {
		res = mir3da_is_ready();
		if (res)
			break;
		wait_count -= DA217_WAIT_INTERVAL;
	} while (wait_count > 0);

	if (res != 1)
		return -1;
#endif

	*x = (int16_t)(tmp_data[1] << 8 | tmp_data[0]) >> 4;
	*y = (int16_t)(tmp_data[3] << 8 | tmp_data[2]) >> 4;
	*z = (int16_t)(tmp_data[5] << 8 | tmp_data[4]) >> 4;
	printf("x: 0x%hx, y: 0x%hx, z: 0x%hx\n", *x, *y, *z);

	return 0;
}

int mir3da_get_step(uint16_t *step)
{
	uint8_t tmp_data[2] = {0};

	if (step == NULL) {
		return -1;
	}

	if (gsreg_multi_read(NSA_REG_STEPS_MSB, tmp_data, 2) != 0) {
		return -1;
	}

	*step = ((uint16_t)(tmp_data[0] << 8 | tmp_data[1])) / 2;
	printf("steps: 0x%hx\n", *step);

	return 0;
}

int mir3da_init(void)
{
	uint8_t data = 0;
	int res = 0;

	res = gsreg_read(NSA_REG_WHO_AM_I, &data);
	if(res != 0 || data != 0x13){
		_Error_Handler(__FILE__, __LINE__);
		return res;
	}

	res |= gsreg_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x83);
	res |= gsreg_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0x69);
	res |= gsreg_mask_write(NSA_REG_ENGINEERING_MODE, 0xFF, 0xBD);

	res |= gsreg_read(0X8E, &data);
	if(data == 0)
		res |= gsreg_mask_write(0X8E, 0xFF, 0x50);

	res |= gsreg_mask_write(NSA_REG_G_RANGE, 0xFF, 0x40);
	res |= gsreg_mask_write(NSA_REG_INT_PIN_CONFIG, 0xFF, 0x00);
	res |= mir3da_set_enable(1);
	res |= gsreg_mask_write(NSA_REG_ODR_AXIS_DISABLE, 0xFF, 0x07);
	res |= mir3da_open_step_counter();
	res |= gsreg_mask_write(NSA_REG_INTERRUPT_MAPPING2, 0xff, 0x04);
	res |= gsreg_mask_write(NSA_REG_INTERRUPT_SETTINGS0, 0xff, 0x04);
	
	return res;
}